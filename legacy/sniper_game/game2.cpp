// same as game but should be a bit faster for bigger maps

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <random>
#include <algorithm>
#include <iterator>
#include <functional>
#include <cmath>


using std::string;
using std::cout;


class Rnd
{
        std::default_random_engine reng;
        std::uniform_real_distribution<double> dist;
        std::function<double()> rnd;

    public:

        static unsigned long seed;

        Rnd(): reng(seed), dist(0, 1), rnd( std::bind(dist, reng) ) {}
        double operator()() { return rnd(); }

        void reseed(unsigned long s) { reng.seed(s); }
};
unsigned long Rnd::seed = 0;
Rnd * rnd = nullptr;

struct Pos
{
    int x, y;
    Pos() {}
    Pos(int a, int b): x(a), y(b) {}
    Pos(string);
    string str() const;
};

bool operator==(const Pos & a, const Pos & b) { return a.x == b.x && a.y == b.y; }
bool operator!=(const Pos & a, const Pos & b) { return !(a == b); }
std::ostream & operator<<(std::ostream & o, const Pos & a) { return o << a.str(); }

double dist2(Pos a, Pos b)
{
    double x = a.x - b.x;
    double y = a.y - b.y;
    return x * x + y * y;
}

class Field;
class Soldier
{
        Pos pos;
        Pos next;
        double accuracy;
        double stealth;
        double speed;
        int fear = 1;

        bool dead = false;
        bool dying = false;

        char team;
        int name;

        friend class Field;

        Soldier(Pos b, double a, double s, double v, int f, char t, int n):
            pos(b), accuracy(a), stealth(s), speed(v), fear(f), team(t), name(n) {}

        string nm() const { return string() + team + char('A' + char(name > 25 ? 25 : name)); }

        double danger(Pos pos, const std::vector<Soldier> & enemies) const;
        string move(Pos sz, Pos b, const std::vector<Soldier> & enemies, bool prn_move);
        string shoot(std::vector<Soldier> & enemies, bool prn_shoot);
};

class Field
{
        Pos size;
        Pos baseB;
        Pos baseR;
        std::vector<Soldier> blues;
        std::vector<Soldier> reds;
        int turn = 0;
        int rep = 0;

        void init(string filename);
        bool arein(Pos base, const std::vector<Soldier> & s) const;
        bool alldead(const std::vector<Soldier> & s) const;
        int survived(const std::vector<Soldier> & s) const;

    public:
        int prn_map = 1;
        int prn_result = 1;
        int prn_title = 1;
        int prn_move = 1;
        int prn_shoot = 1;

        int Nrep = 1;
        void reset();

        Field(string filename) { init(filename); }
        string title() const;
        string map() const;

        string move();
        string shoot();
        int isdone() const; // 0 go on; 1 - blues; 2 - reds; 3 draw
        string result(string = "") const;

        int aliveR() const { return survived(reds); }
        int aliveB() const { return survived(blues); }
};

int main(int ac, char * av[])
try
{
    Field field("game.conf");

    Rnd rnd_main; rnd = &rnd_main;

    int stat[5] = {0, 0, 0, 0, 0};

    for ( int k = 0; k < field.Nrep; k++ )
    {
        field.reset();

        cout << field.title();

        int done = 0;

        for ( int i = 0; i < 10000; i++ )
        {
            cout << field.move();
            cout << field.map();

            int ar = field.aliveR();
            int ab = field.aliveB();

            string kills = field.shoot();
            if ( !kills.empty() )
            {
                cout << kills << '\n';

                cout << "Killed/Survived: "
                     << (ab - field.aliveB()) << "/" << (field.aliveB()) << " blues, "
                     << (ar - field.aliveR()) << "/" << (field.aliveR()) << " reds\n";

                cout << field.map();
            }

            done = field.isdone();
            if ( done ) break;
        }

        cout << field.result("game.log");
        stat[done]++;

    }

    cout << "Not finished  : " << stat[0] << '\n';
    cout << "Won by Blues  : " << stat[1] << '\n';
    cout << "Won by Reds   : " << stat[2] << '\n';
    cout << "Draw all dead : " << stat[3] << '\n';
    cout << "Draw both win : " << stat[4] << '\n';
}
catch (string e)
{
    cout << "Error: " << e << '\n';
    return 1;
}
catch (...)
{
    cout << "Error\n";
    return 2;
}

void Field::init(string filename)
{
    std::ifstream in(filename.c_str());

    if ( !in )
        throw "Cannot open " + filename;

    std::map<string, string> dict;

    int mode = 0; // 0-none, 1-blue, 2-red

    size = Pos(-1, -1);
    Pos base[3];
    int ns[3] = {0, 0, 0};
    double acc[3];
    double stl[3];
    double vel[3];
    double fea[3];

    string s;
    for (; in >> s;)
    {
        std::istringstream in(s);
        string k, v;
        std::getline(in, k, '=');
        std::getline(in, v);

        if ( k.empty() ) throw "corrupted " + filename;

        for ( int i = 0; i < 100; i++ )
        {
            void replaceAll(string & s, const string & r, const string & to);
            for ( auto j : dict )
                replaceAll(v, j.first, j.second);
        }

        if (0);
        else if ( k[0] == '#' );
        else if ( k[0] == '$' ) dict[k] = v;
        else if ( k == "[blue]" ) mode = 1;
        else if ( k == "[red]" ) mode = 2;
        else if ( k == "field" ) size = Pos(v);
        else if ( k == "base" ) base[mode] = Pos(v);
        else if ( k == "N" ) ns[mode] = std::atoi(v.c_str());
        else if ( k == "accuracy" ) acc[mode] = std::atof(v.c_str());
        else if ( k == "stealth" ) stl[mode] = std::atof(v.c_str());
        else if ( k == "speed" ) vel[mode] = std::atof(v.c_str());
        else if ( k == "fear" ) fea[mode] = std::atof(v.c_str());
        else if ( k == "seed" ) Rnd::seed = std::atol(v.c_str());
        else if ( k == "nrep" ) Nrep = std::atol(v.c_str());

        else if ( k == "prn_map" ) prn_map = std::atol(v.c_str());
        else if ( k == "prn_result" ) prn_result = std::atol(v.c_str());
        else if ( k == "prn_title" ) prn_title = std::atol(v.c_str());
        else if ( k == "prn_move" ) prn_move = std::atol(v.c_str());
        else if ( k == "prn_shoot" ) prn_shoot = std::atol(v.c_str());

        else
            throw "Unexpected [" + k + "] in " + filename;
    }

    if ( size.x < 3 ) throw "Field size undefined in " + filename;

    baseB = base[1];
    baseR = base[2];

    for ( int i = 0; i < ns[1]; i++ )
        blues.push_back(Soldier(baseB, acc[1], stl[1], vel[1], fea[1], 'B', i));

    for ( int i = 0; i < ns[2]; i++ )
        reds.push_back(Soldier(baseR, acc[2], stl[2], vel[2], fea[2], 'R', i));
}

void replaceAll(string & s, const string & r, const string & to)
{
    while (1)
    {
        size_t i = s.find(r);
        if ( i == string::npos ) return;
        s.replace(i, r.size(), to);
    }
}

Pos::Pos(string s)
{
    std::istringstream i(s);
    char a;
    i >> x >> a >> y;
}

string Pos::str() const
{
    std::ostringstream o;
    o << x << ',' << y;
    return o.str();
}

string Field::title() const
{
    if ( !prn_title ) return "";

    std::ostringstream o;

    o << "rep=" << rep;

    o << " size=" << size.str() << " baseB=[" << baseB.str() << "] baseR=[" << baseR.str() << "]";
    if ( !blues.empty() )
    {
        o << " accB=" << blues[0].accuracy;
        o << " stlB=" << blues[0].stealth;
        o << " velB=" << blues[0].speed;
    }

    if ( !reds.empty() )
    {
        o << " accR=" << reds[0].accuracy;
        o << " stlR=" << reds[0].stealth;
        o << " velR=" << reds[0].speed;
    }

    o << '\n';

    return o.str();
}

string Field::map() const
{
    if ( !prn_map ) return "";

    std::ostringstream o;

    for ( int i = 0; i < size.x; i++ ) o << "--"; o << '\n';

    //B R M ! a-z #
    std::vector<string> v;
    for ( int j = 0; j < size.y; j++ )
        for ( int i = 0; i < size.x; i++ )
            v.push_back("  ");

    Pos sz = size;
    auto c = [&v, sz](Pos z) -> string&
    {
        return v[(z.y - 1) * sz.x + (z.x - 1)];
    };

    c(baseB) = "B!";
    c(baseR) = "R!";

    for ( int i = 0; i < blues.size(); i++ )
    {
        if ( blues[i].dead ) continue;
        string & s = c(blues[i].pos);
        if ( s[0] == ' ' ) s = blues[i].nm();
        else if ( s[0] == 'B' ) s = "B#";
        else s = "M#";
    }

    for ( int i = 0; i < reds.size(); i++ )
    {
        if ( reds[i].dead ) continue;
        string & s = c(reds[i].pos);
        if ( s[0] == ' ' ) s = reds[i].nm();
        else if ( s[0] == 'R' ) s = "R#";
        else s = "M#";
    }

    for ( int j = 0; j < size.y; j++ )
    {
        for ( int i = 0; i < size.x; i++ )
            o << v[i + j * sz.x];
        o << '\n';
    }

    for ( int i = 0; i < size.x; i++ ) o << "--";
    o << '\n';
    return o.str();
}

string Field::move()
{
    turn++;

    std::ostringstream o;

//    blues[0].move(size,baseR,reds);
    for ( auto & i : blues ) o << i.move(size, baseR, reds, prn_move);
    for ( auto & i : reds ) o << i.move(size, baseB, blues, prn_move);

    for ( auto & i : blues ) i.pos = i.next;
    for ( auto & i : reds ) i.pos = i.next;

    return prn_move ? o.str() + '\n' : "";
}

string draw(std::vector<Pos> cells)
{
    int m = 0;
    int r = 0;
    for ( auto i : cells )
    {
        if ( i.x > m ) m = i.x;
        if ( i.y > r ) r = i.y;
    }

    std::vector<string> v;
    for ( int j = 0; j < r; j++ )
        for ( int i = 0; i < m; i++ )
            v.push_back("  ");

    auto c = [&v, m](Pos z) -> string& { return v[(z.y - 1) * m + (z.x - 1)]; };

    for ( auto i : cells )
        c(i) = "[]";

    std::ostringstream o;
    for ( int j = 0; j < r; j++ )
    {
        for ( int i = 0; i < m; i++ )
            o << v[i + j * m];
        o << '\n';
    }

    return o.str();
}

double Soldier::danger(Pos ps, const std::vector<Soldier> & enemies) const
{
    double p = 1;

    const double s2 = stealth * stealth;
    for ( auto j : enemies )
    {
        if ( j.dead ) continue;
        double r2 = dist2(ps, j.pos);
        p *= 1 - accuracy * std::exp( -r2 / s2 );
    }

    double q = 1;
    for ( int j = 0; j < fear; j++ ) q *= p;

    return 1 - q;
}

string Soldier::move(Pos sz, Pos base, const std::vector<Soldier> & enemies, bool prn_move)
{
    if ( dead )
    {
        next = pos;
        return "";
    }

    // first collect the list of all possible places

    std::multimap<double,Pos> possible;

    //std::vector<Pos> possible;

    double sp2 = speed * speed;
    for ( int j = pos.y - speed; j <= pos.y + speed; j++ )
        for ( int i = pos.x - speed; i <= pos.x + speed; i++ )
        {
            if ( i < 1 || j < 1 || i > sz.x || j > sz.y ) continue;

            double d2 = dist2(Pos(i, j), pos);

            if ( d2 > sp2 + 0.1 ) continue;

            d2 = dist2(Pos(i, j), base);
            possible.insert( std::pair<double,Pos>(d2,Pos(i, j)) );
        }

    bool found = false;
    for ( auto i : possible )
    {
        double dang = danger(i.second, enemies);
        if ( dang < (*rnd)() )
        {
            next = i.second;
            found = true;
            break;
        }
    }

    if ( !found ) // panic
    {
        next = pos;
        return "";
    }

    if ( !prn_move ) return "";

    std::ostringstream o;

    if ( next != pos )
        o << "(" << nm() << ":" << pos << ">" << next << ")";

    return o.str();
}


string Field::shoot()
{
    std::ostringstream o;

    for ( auto & i : blues ) o << i.shoot(reds, prn_shoot);
    for ( auto & i : reds ) o << i.shoot(blues, prn_shoot);

    for ( auto & i : blues ) i.dead = i.dying;
    for ( auto & i : reds ) i.dead = i.dying;

    return o.str();
}

string Soldier::shoot(std::vector<Soldier> & enemies, bool prn_shoot)
{
    if ( dead )
    {
        next = pos;
        return "";
    }

    std::ostringstream o;

    for ( auto & i : enemies)
    {
        if ( i.dead || i.dying ) continue;
        i.dying = false;

        double s2e = i.stealth; s2e *= s2e;

        double r2 = dist2(pos, i.pos);
        double p = accuracy * std::exp( -r2 / s2e );

        if ( (*rnd)() > p ) continue; // no kill

        if (prn_shoot) o << "(" << nm() << "/" << i.nm() << ")";
        i.dying = true;
    }

    return o.str();
}

int Field::isdone() const
{
    bool rdead = alldead(reds);
    bool bdead = alldead(blues);

    if ( rdead && bdead ) return 3;
    if ( rdead && !bdead ) return 1;
    if ( !rdead && bdead ) return 2;

    bool rin = arein(baseB, reds);
    bool bin = arein(baseR, blues);

    if ( !rin && !bin ) return 0;

    bool rdef = arein(baseR, reds);
    bool bdef = arein(baseB, blues);

    if ( rin && bin && !rdef && !bdef ) return 4;

    if ( rin && !bdef ) return 2;
    if ( bin && !rdef ) return 1;

    return 0;
}

bool Field::alldead(const std::vector<Soldier> & s) const
{
    for ( const auto & i : s )
        if ( !i.dead ) return false;

    return true;
}

int Field::survived(const std::vector<Soldier> & s) const
{
    int k = 0;
    for ( const auto & i : s )
        if ( !i.dead ) k++;

    return k;
}

bool Field::arein(Pos base, const std::vector<Soldier> & s) const
{
    for ( const auto & i : s )
    {
        if ( i.dead ) continue;
        if ( base == i.pos ) return true;
    }

    return false;
}

string Field::result(string log) const
{
    int end = isdone();

    std::ostringstream o;
    switch (end)
    {
        case 0: o << "Result: Not finished\n"; break;
        case 1: o << "Result: Blues win\n"; break;
        case 2: o << "Result: Reds win\n"; break;
        case 3: o << "Result: Draw (all dead)\n"; break;
        case 4: o << "Result: Draw (both sides win)\n"; break;
        default:
            throw string() + "Internal error 525";
    }

    int alB = aliveB();
    int alR = aliveR();
    int casB = (blues.size() - alB);
    int casR = (reds.size() - alR);

    o << "Casualties Blues: " << casB << " of " << blues.size() << '\n';
    o << "Casualties Reds : " << casR << " of " << reds.size() << '\n';

    o << "Turns : " << turn << '\n';

    if ( !log.empty() )
    {
        std::ofstream of(log.c_str(), std::ios::app);

        string s = ",   ";
        of << Rnd::seed << s << rep << s << end << s << casB << s
           << alB << s << casR << s << alR << s << turn << '\n';
    }

    return prn_result ? o.str() : "";
}

void Field::reset()
{
    rep++;
    turn = 0;

    for ( auto & i : blues ) i.pos = i.next = baseB;
    for ( auto & i : reds ) i.pos = i.next = baseR;
    for ( auto & i : blues ) i.dead = i.dying = false;
    for ( auto & i : reds ) i.dead = i.dying = false;
}



