#include <iostream>
#include <map>
#include <cmath>

#include "os_timer.h"
#include "os.h"

#include "gl_utils.h"
#include "quest.h"

double stepf(double threshold, double x)
{
    const int s = 4;
    double ts = std::pow(threshold, s);
    double xs = std::pow(x, s);
    return xs / (xs + ts);
}

void Quest::update(float x)
{
    Vf & vf = pl->items[item].hist[quse][ip];
    vf.push_back(Point(x, now()));
    if ( (int)vf.size() > HIST ) vf.erase(vf.begin());
}

void Quest::expect(uchar c, double badk_penalty, double time_window)
{
    os::Timer tm;
    for (int i = 0; ; i++ )
    {
        // eat all stale keystrokes
        {
            int adj = 0;
            do { adj = os::kbhit(); adjust_prn(adj); }
            while (adj);
        }

        uchar k = 0;
        if ( (time_window -= tm.get()) > 0 )
            k = getcode(time_window);

        if ( k == uchar(27) || k == uchar(0) )
        {
            qstat[1]++;
            return update(2 * esc_penalty);
        }
        if ( k == c )
        {
            qstat[0]++;
            return update((float)(tm.get() + i * badk_penalty) );
        }

        if ( adjust_prn(k) ) continue; // do not penalize

        // penalise only after thinking
        double coef = 1;
        if ( time_window > 1e-6 )
            coef = stepf(0.2, tm.get() / time_window);
        time_window -= badk_penalty * coef;
    }
}

Quest::Quest(Lesson * les, int itm, int q)
    : pl(les), item(itm), ip(0), quse(q), qstat {0, 0}
{
}

bool Quest::adjust_prn(int k)
{
    Edu & e = pl->edu;
    if ( k == 45 ) e.prndelay += 1 + e.prndelay / 4;
    else if ( k == 61 ) e.prndelay -= e.prndelay / 5;
    else return false;
    return true;
}

void Quest::ask(bool letsel2)
{
    string ot = pl->items[item].otext;
    string se;

    const Edu & edu = pl->edu;

    if ( letsel2 ) se = selector2(edu);
    else se = selector1(edu);

    if ( se.size() != ot.size() ) throw "bad se";

    bool w = false;
    int j = 0;
    int width = os::termSize().first - WINDOW_MARGIN;
    while ( !done() )
    {
        char oc = ot[ip];
        uchar c = se[ip];
        if ( c != '.' )
        {
            ///char pr = '_';
            ///if ( !last() && se[ip + 1] != '.' ) pr = '.';
            expect(c, edu.bkpen * 1000, edu.level * 2000);
            ///expect(c, edu.bkpen * 1000, pr);
            w = false;
        }
        else
        {
            if ( w ) os::sleep(edu.prndelay);
            w = true;
            auto k = os::kbhit();
            //if ( k > 0 ) std::cout << "[" << k << "]" << std::flush;
            ///if ( k == 45 ) edu.prndelay += 1 + edu.prndelay / 4;
            ///if ( k == 61 ) edu.prndelay -= edu.prndelay / 5;

            adjust_prn(k);
        }

        if ( ( ++j > width && oc == ' ' ) || oc  == '\n' )
        {
            std::cout << '\n';
            j = 0;
        }
        else
            std::cout << oc << std::flush;

        ip++;
    }
}

bool Quest::done() const
{
    return ip >= (int)pl->items[item].otext.size();
}

bool Quest::last()
{
    ++ip;
    bool r = done();
    --ip;
    return r;
}

string Quest::selector2(Edu edu)
{
    Item & it = pl->items[item];
    string r(it.otext.size(), '.');
    string s = it.otext;

    if (0);
    else if ( it.st.badans.size() || it.st.outside.size() )
///    {
///        it.updatestat(now(), edu);
///        for ( auto i : it.st.badans ) r[i] = askable(s[i]);
///        for ( auto i : it.st.outside ) r[i] = askable(s[i]);
///    }

///    else if ( it.st.outside.size() )
    {
        it.updatestat(now(), edu);
        for ( auto i : it.st.badans ) r[i] = askable(s[i]);
        for ( auto i : it.st.outside ) r[i] = askable(s[i]);

        string a = r;
        while (1)
        {
            if ( a.empty() ) throw "Bad otext";

            for ( int i = 2; i < int(a.size()); i++ )
            {
                auto & on = it.st.soon;
                using gl::isin;
                if (  isin(on, i) && isin(on, i - 1) && isin(on, i - 2) )
                    a[i] = askable(s[i]);
            }

            for ( int i = 1; i < int(a.size() - 1); i++ )
            {
                for ( int j = i - 1; j <= i + 1; j += 2 )
                    if ( a[i] != '.' && gl::isin(it.st.soon, j) )
                        a[j] = askable(s[j]);
            }

            if ( a == r ) break;
            r = a;
        }
    }

    else if ( it.st.notasked.size() )
        for ( auto i : it.st.notasked ) r[i] = askable(s[i]);

    //else return selector1(edu);
    else
    {
        Edu nedu = edu;
        nedu.stretch /= 2;
        if ( nedu.stretch < 0.1 ) return selector1(edu);
        it.updatestat(now(), nedu);
        r = selector2(nedu);
        it.updatestat(now(), edu);
    }

    return r;
}

string Quest::selector1(Edu edu)
{
    // this function selects letter to ask according to diff and rates

    string hms = os::Timer::getHms();
    unsigned urnd = 103 * hms[0] + 7 * hms[1] + 11 * hms[2]
                    + 13 * hms[3] + 17 * hms[4] + 29 * hms[5];
    static unsigned surnd = 0;

    string s = pl->items[item].otext;

    std::map<double, int> m;
    string r;

    for ( size_t i = 0; i < s.size(); i++ )
    {
        r += '.';
        if ( !askable(s[i]) ) continue;

        double rt = pl->items[item].rate(quse, int(i));
        double rnd = 0.000001 * ((surnd += urnd) % 1023);
        double rn2 = 0.0001 * (i % 1023);
        double key = -rt - rnd - rn2;
        m[key] = (int)i;
    }

    if ( pl->diff == 0 || m.empty() ) return r;

    int sz = int(1.0 * m.size() * pl->diff / 100 + 0.5);
    if ( !sz ) sz = 1;

    while (sz && !m.empty() )
    {
        int i = m.begin()->second;
        r[i] = askable(s[i]);
        m.erase(m.begin());
        sz--;
    }

    return r;
}

void Item::seennow()
{
    for ( auto & i : hist[0] ) // choose letter
    {
        if ( i.empty() ) throw "Item history empty";
        int z = (int)i.size() - 1;
        if ( i[z].t < 0 ) continue; // non-printable
        if ( i[z].t < esc_penalty ) continue; // ignore
        i[z].s = now();
    }
}

bool Lesson::updatestat(double tim)
{
    st.badans = 0;
    st.inside = 0;
    st.outside = 0;
    st.soon = 0;
    st.notasked = 0;
    st.minfuture = 1e100;

    for ( auto & i : items )
    {
        if ( !i.updatestat(tim, edu) ) return false;

        st.badans += (int)i.st.badans.size();
        st.inside += (int)i.st.inside.size();
        st.outside += (int)i.st.outside.size();
        st.soon += (int)i.st.soon.size();
        st.notasked += (int)i.st.notasked.size();

        if ( st.minfuture > i.st.minfuture )
            st.minfuture = i.st.minfuture;
    }

    return true;
}

bool Item::updatestat(double tim, Edu edu)
{
    st.badans.clear();
    st.inside.clear();
    st.outside.clear();
    st.soon.clear();
    st.notasked.clear();
    st.minfuture = 1e100;

    for ( int i = 0; i < (int)hist[0].size(); i++ ) // choose letter
    {
        Vf & it = hist[0][i];
        if ( it.empty() ) throw "Item history empty";
        if ( it[0].t < 0 ) continue; // non-printable

        int z = (int)it.size() - 1;

        //if ( it[z].s < 0 ) return false; // disallow no preview

        if ( it[z].t == esc_penalty )
        {
            st.notasked.push_back(i);
            continue;
        }

        if ( it[z].t > esc_penalty )
        {
            st.badans.push_back(i);
            continue;
        }

        double maxrepl;

        if ( edu.level > 0 ) maxrepl = getperedu(it, edu);
        else maxrepl = getperiod(it, edu);

        double futuretime = maxrepl + it[z].s;

        if ( st.minfuture > futuretime )
            st.minfuture = futuretime;

        if ( tim < futuretime )
            st.inside.push_back(i);
        else
            st.outside.push_back(i);

        // now do for soon
        if ( edu.level > 0 ) maxrepl = getperedu(it, edu.shrink());
        else maxrepl = getperiod(it, edu.shrink());

        futuretime = maxrepl + it[z].s;

        if ( tim > futuretime )
            st.soon.push_back(i);
    }

    return true;
}


double Item::getperiod(Vf & vf, Edu edu)
{
    double r = 0;
    int sz = (int)vf.size();
    for ( int i = sz - 2; i >= 0; i-- )
    {
        if (vf[i + 1].t >= esc_penalty) break;
        if (vf[i + 1].s < 0) break;
        if (vf[i].s < 0) break;
        double x = vf[i + 1].s - vf[i].s;
        if ( x > r ) r = x;
    }

    return r * edu.stretch;
}


double Item::getperedu(Vf & vf, Edu edu)
{
    double r = 100000; // 86400=one day
    //r *= 7;

    int sz = (int)vf.size();
    for (int i = 1; i < sz; i++)
    {
        double a = 0;
        if (vf[i - 1].s < 0)
        {
            if (vf[i].s < 0) continue;
        }
        else
            a = vf[i].s - vf[i - 1].s;

        double f = stepf(edu.level * 1000, vf[i].t);

        double b = (a - r) / 2;
        if (b < 0) b = 0;

        r += (1 - f) * edu.stretch * (a + b + edu.boncon) - f * (r * 3 / 4 );
    }

    return r;
}

