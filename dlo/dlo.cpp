#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>

using namespace std;

typedef vector<char> vch;

struct command
{

    enum cmd_type { block = 0xBB, symbol = 0xAA, str = 0xFF };

    cmd_type cmd;
    union
    {
        unsigned long n1;
        char c;
        vch * v;
    };
    unsigned long n2;

    command(cmd_type cc, unsigned long a1): cmd(cc), n1(a1) {}
    command(cmd_type cc, char a1): cmd(cc), c(a1) {}
    command(cmd_type cc, vch * a1): cmd(cc), v(a1) {}
    command(cmd_type cc): cmd(cc) {}
    command(cmd_type cc, unsigned long a1, unsigned long a2):
        cmd(cc), n1(a1), n2(a2) {}
};

typedef vector<command> vcm;

class vch_holder
{
        vcm * p;
        vector<vch *> vchs;
        vch_holder(vch_holder &);
    public:
        vch * getvch()
        {
            vch * p = new vch;
            vchs.push_back(p);
            return p;
        }
        ~vch_holder()
        {
            while ( !vchs.empty() )
            {
                delete vchs.back();
                vchs.pop_back();
            }
        }
        vch_holder(vcm & c): p(&c) {}
};


unsigned long load(ifstream & is, vch & v);
bool loaddlo(ifstream & is, vcm & v, vch_holder & h);
void getdlo(vch & f1, vch & f2, vcm & d);
void procdlo(vcm & d, vch_holder & vh, vch & file);
void savedlo(ofstream & of, vcm & d);
// imia, razmer, bsumma
void dlomake(vch & out, vcm & d, vch * in);
void save(ofstream & of, const vch & file);


bool read_int(istream & i, unsigned long & x);
bool read_string(istream & i, string & s);


void print_bar(long x, long max)
{
    if ( x > max ) x = max;
    if ( x < 0 ) x = 0;
    const long bar_size = 50;
    int bar = int( (float)x / max * 50 );

    static int oldbar = -1;
    if ( oldbar == bar ) return;
    oldbar = bar;

    cout << '\r';
    long i;
    for ( i = 0; i < bar; i++ ) cout << 'X';
    for ( ; i < bar_size; i++ ) cout << '.';
    cout << flush;
}


int main(int ac, char * av[])
{

    if ( ac < 3 ) goto bad;

    if ( string("delta") == av[1] )
    {

        if ( ac != 5 && ac != 6 ) goto bad;
        ifstream if1(av[2], ios::binary);
        ifstream if2(av[3], ios::binary);
        ofstream of(av[4], ios::binary);

        if ( !if1 )
        {
            cout << "Cannot open " << av[2] << endl;
            return 2;
        }
        else if ( !if2 )
        {
            cout << "Cannot open " << av[3] << endl;
            return 3;
        }
        else if ( !of )
        {
            cout << "Cannot open " << av[4] << endl;
            return 4;
        }

        vch f1;
        unsigned long bsum1 = load(if1, f1);
        vch f2;
        unsigned long bsum2 = load(if2, f2);
        vcm d;
        getdlo(f1, f2, d);
        vch_holder holder(d);
        procdlo(d, holder, f1);

        of << av[2] << '\n' << f1.size() << '\n' << bsum1 << '\n';
        of << av[3] << '\n' << f2.size() << '\n' << bsum2 << '\n';
        savedlo(of, d);

        if (ac == 6)
        {
            ofstream of2(av[5], ios::binary);
            if ( !of )
            {
                cout << "Cannot open " << av[5] << endl;
                return 5;
            }
            vcm d2;
            getdlo(f2, f1, d2);
            vch_holder holder2(d2);
            procdlo(d2, holder2, f2);
            of2 << av[3] << '\n' << f2.size() << '\n' << bsum2 << '\n';
            of2 << av[2] << '\n' << f1.size() << '\n' << bsum1 << '\n';
            savedlo(of2, d2);

        }// ac==6

    }
    else if ( string("make") == av[1] )
    {

        ifstream in(av[2], ios::binary);
        if ( !in )
        {
            cout << "Cannot open " << av[2] << endl;
            return 2;
        }
        vcm d;
        vch_holder hd(d);
        string name1, name2;
        unsigned long bsum1, bsum2, size1, size2;
        read_string(in, name1);
        read_int(in, size1);
        read_int(in, bsum1);
        read_string(in, name2);
        read_int(in, size2);
        read_int(in, bsum2);

        bool r = loaddlo(in, d, hd);
        if ( !r )
        {
            cout << "Error loading " << av[2] << endl;
            return 7;
        }

        bool noblocks = true;
        {
            vcm::iterator it = d.begin();
            for ( ; it != d.end(); it++ )
                if ( (*it).cmd != command::str &&
                        (*it).cmd != command::symbol )
                {
                    noblocks = false;
                    break;
                }
        }

        vch f2;
        if ( noblocks )
        {
            dlomake(f2, d, 0);
        }
        else
        {
            if ( ac < 4 )
            {
                cout << "dlo requires input file " <<
                     ", specify filename with additional "
                     "argument" << endl;
                return 1;
            }
            ifstream i1(av[3], ios::binary);
            if ( !i1 )
            {
                cout << "Cannot open " << av[3] << endl;
                return 2;
            }

            vch f1;

            unsigned long bs1 = load(i1, f1);
            if ( f1.size() != size1 || bs1 != bsum1 )
            {
                cout << "size or bsum do not match dlo "
                     "file (input file incorrect)" << endl;
                return 10;
            }

            dlomake(f2, d, &f1);

            unsigned long bs2 = 0;
            for (vch::iterator vi = f2.begin(); vi != f2.end(); ++vi)
                bs2 += (unsigned char) * vi;
            if ( f2.size() != size2 || bs2 != bsum2 )
            {
                cout << "size or bsum do not match dlo "
                     "file (dlo corrupted)" << endl;
                return 11;
            }
        }

        ofstream of(name2.c_str(), ios::binary);
        if ( !of )
        {
            cout << "Cannot open " << name2.c_str() << endl;
            return 4;
        }

        save(of, f2);

    }
    else goto bad;


    return 0;
bad:
    cout << "Use: delta infile1 infile2 "
         "outfile1.dlo, [outfile2.dlo],\n"
         "or, make dlofile [infile]" << endl;
    return 1;
}

// -------------------------------------------------------------

unsigned long load(ifstream & is, vch & v)
{

    char c;
    unsigned long s = 0;
    while (1)
    {
        is.get(c);
        if ( !is ) break;
        s += (unsigned char)c;
        v.push_back(c);
    }
    return s;
}

unsigned long b72ul(const vch & b7)
{
    unsigned long r = 0;
    int i = b7.size() - 1;
    while (1)
    {
        r += (unsigned char)(b7[i] & 0x7f);
        if ( i <= 0 ) break;
        r <<= 7;
        i--;
    }
    return r;
}

vch ul2b7(unsigned long x)
{
    vch r;
    do
    {
        r.push_back( char(x & 0x7F | 0x80) );
        x >>= 7;
    }
    while (x);
    r.back() &= 0x7F;
    return r;
}

vch getb7(ifstream & i)
{
    vch r;
    char c;
    while (1)
    {
        i.get(c);
        if ( !i ) break;
        r.push_back(c);
        if ( !(c & 0x80) ) break;
    }
    return r;
}

void savedlo(ofstream & of, vcm & d)
{

    for (vcm::iterator i = d.begin(); i != d.end(); ++i )
        switch ( (*i).cmd )
        {
            case command::block:
                of << char(command::block);
                save( of, ul2b7((*i).n1) );
                save( of, ul2b7((*i).n2 - (*i).n1) );
                break;
            case command::symbol:
                of << char(command::symbol);
                of << (*i).c;
                break;
            case command::str:
                of << char(command::str);
                save( of, ul2b7((*i).v->size()) );
                save(of, *(*i).v);
                break;

            default:
                cout << "Bad dlo" << endl;
                return;
        }
}

bool loaddlo(ifstream & is, vcm & v, vch_holder & h)
{

    char c;
    while (1)
    {
        is.get(c);
        if ( !is ) break;
        switch ( command::cmd_type((unsigned char)c) )
        {
            case command::block:
            {
                unsigned long n1 = b72ul( getb7(is) );
                unsigned long n2 = b72ul( getb7(is) );
                v.push_back( command(command::block, n1, n1 + n2) );
                ///cout<<"block "<<n1<<' '<<n2<<endl;
            }
            break;
            case command::symbol:
                is.get(c);
                v.push_back( command(command::symbol, c) );
                ///cout<<"symbol "<<c<<endl;
                break;
            case command::str:
            {
                unsigned long size = b72ul( getb7(is) );
                vch * p = h.getvch();
                for ( unsigned long j = 0; j < size; j++ )
                {
                    is.get(c);
                    p->push_back(c);
                }
                v.push_back( command(command::str, p) );
            }
            break;

            default: cout << "c: " << c << endl; goto bad;
        }
        if ( !is ) goto bad;
    }

    return true;
bad:
    cout << "dlo file corrupted" << endl;
    return false;
}

const unsigned long LIMIT_SIZE = 10;

bool isunder(command c)
{
    if ( c.cmd == command::block ) return c.n2 < c.n1 + LIMIT_SIZE;
    if ( c.cmd == command::symbol ) return true;

    return false;
}

command combine(
    vcm::iterator st_lim, vcm::iterator it,
    vch_holder & h, vch & f)
{
    vch vv;
    do
    {
        if ( (*st_lim).cmd == command::symbol )
            vv.push_back((*st_lim).c);
        if ( (*st_lim).cmd == command::block )
            for ( unsigned long k = (*st_lim).n1;
                    k < (*st_lim).n2; ++k)
                vv.push_back(f[k]);
    }
    while ( ++st_lim != it );
    vch * p = h.getvch();
    vv.swap(*p);
    return command(command::str, p);
}

void procdlo(vcm & d, vch_holder & h, vch & f)
{
// mark all blocks below LIMIT_SIZE and more then one
// combine into strings

    vcm dd;
    vch vv;

    vcm::iterator st_lim = d.end();
    for ( vcm::iterator it = d.begin(); it != d.end(); ++it )

        if ( isunder(*it) )
            if ( st_lim == d.end() )
            {
                st_lim = it;
            }
            else
            {
            }
        else if ( st_lim == d.end() )
        {
            dd.push_back(*it);
        }
        else if ( ++st_lim == it )
        {
            // check if the prev was only 1
            dd.push_back(*--st_lim);
            dd.push_back(*it);
            st_lim = d.end();
        }
        else
        {
            --st_lim;
            command com = combine(st_lim, it, h, f);
            dd.push_back( com );

            dd.push_back(*it);
            st_lim = d.end();
        }

    if ( st_lim != d.end() )
    {
        command com = combine(st_lim, d.end(), h, f);
        dd.push_back( com );
    }

    d.swap(dd);
}

void dlomake(vch & out, vcm & d, vch * in)
{

    for (vcm::iterator i = d.begin(); i != d.end(); ++i )
        switch ( (*i).cmd )
        {
            case command::block:
                ///cout<<"block "<<(*i).n1<<' '<<(*i).n2<<endl;
                if (in)
                    if ( in->size() < (*i).n2 )
                    {
                        cout << "out of bound (dlo does not match "
                             "input file) [size:" << in->size() <<
                             ", ref:" << (*i).n2 << "]" << endl;
                    }
                    else
                    {
                        for ( unsigned long j = (*i).n1;
                                j < (*i).n2; j++ )
                            out.push_back( (*in)[j] );
                    }
                break;
            case command::symbol:
                ///cout<<"symbol "<<(*i).c<<endl;
                out.push_back( (*i).c );
                break;
            case command::str:
            {
                vch & v = *(*i).v;
                {
                    for ( unsigned long j = 0; j < v.size(); j++ )
                        out.push_back(v[j]);
                }
            }
            break;

            default:
                cout << "Bad dlo 2" << endl;
                return;
        }
}

void save(ofstream & of, const vch & s)
{
    for ( vch::const_iterator i = s.begin(); i != s.end(); i++ )
        of << (*i);
}

typedef unsigned long ulong;

void getdlo(vch & f1, vch & f2, vcm & d)
{

    ulong c2, i, j, k;
    ulong size1 = f1.size();
    ulong size2 = f2.size();

    if ( size2 == 0 ) return;

    c2 = 0;

    map<char, vector<ulong> > mcv;

    for ( i = 0; i < size1; i++ )
        mcv[f1[i]].push_back(i);

    vector<ulong> mat;
    vector<ulong> mat2;

mainloop:

    print_bar(c2, size2);

    if ( c2 == size2 ) return;

    ulong c2fix = c2;

    //mat.clear();
    //for( i=0; i<size1; i++ )
    //if( f1[i] == f2[c2] ) mat.push_back(i);
    mat = mcv[f2[c2]];

    if ( mat.empty() )
    {
        d.push_back( command(command::symbol, f2[c2]) );
        c2++;
        goto mainloop;
    }

matloop:
    mat2.clear();
    c2++;
    if ( c2 != size2 )
        for ( i = 0; i < mat.size(); i++ )
        {

            ulong c1 = mat[i] + 1;
            if ( c1 == size1 ) continue;

            if ( f1[c1] == f2[c2] )
                mat2.push_back(c1);

        }
//  { // eto byla popytka uskorit no okazalos' zamedlelo
//      vector<ulong> & mat3 = mcv[f2[c2]];
//      int i=0, j=0, s1=mat.size(), s2=mat3.size();
//      for(; i!=s1 && j!=s2;)
//      if( mat[i]+1 < mat3[j] ) i++;
//      else if( mat3[j] < mat[i]+1 ) j++;
//      else{
//          mat2.push_back(mat[i]+1); i++; j++;
//      }
//  }

    if ( !mat2.empty() )
    {
        mat.swap(mat2);
        goto matloop;
    }

    ulong len = c2 - c2fix;
    ulong beg = mat[0] + 1 - len;
    d.push_back( command(command::block, beg, beg + len) );

    goto mainloop;
}

/* eto mertvyj kod - popytka bystrogo sravnenoja
mozhet kogda nibud' ego nuzhno budet ozhivit'
poka oblom - on pochti rabotaet
int dlo_fast_chunk = 4;
void getdlo(vch & f1, vch & f2, vcm & d){

    ulong c3, c2, i, j, k;
    ulong size1 = f1.size();
    ulong size2 = f2.size();

    if( size2 == 0 ) return;

    c2 = 0;

    typedef map<ol::thing,vector<ulong> > mtv;
    mtv mp;

    for( i=0; i<size1-dlo_fast_chunk; i+=dlo_fast_chunk )
    mp[ol::thing(&f1[i],dlo_fast_chunk)].push_back(i);

    vector<ulong> mat;
    vector<ulong> mat2;
    mtv::iterator it;

    c3 = c2; // tam gde nachalos' nesootvetstvie

mainloop:

    print_bar(c2,size2-dlo_fast_chunk);

    if( c2 >= size2 - dlo_fast_chunk ) goto finalize;

    ulong c2fix = c2;

    it = mp.find(ol::thing(&f2[c2],dlo_fast_chunk) );
    if( it == mp.end() ){
        c2++;
        goto mainloop;
    }

    mat = it->second;

matloop:

    mat2.clear();
    c2+=dlo_fast_chunk;

    if( c2 < size2 - dlo_fast_chunk )

    for( i=0; i<mat.size(); i++ ){

        ulong c1 = mat[i]+dlo_fast_chunk;
        if( c1 >= size1 - dlo_fast_chunk ) continue;

        if( ol::binary::cmpn(
            (ol::byte*)&f1[c1],
            (ol::byte*)&f2[c2],
            dlo_fast_chunk)==0 )
            mat2.push_back(c1);

    }

    if( !mat2.empty() ){
        mat.swap(mat2);
            goto matloop;
    }

    // tut my imeem  c3 - tam gde nachalos' nesootvetsvie
    // c2fix - tam gde nachalos' sootvetsvie
    // c2 - gde opiat' nessotvetstvie
    // nado podvinut' c2 vpered i c2fix nazad
        // primer d.push_back( command(command::symbol,f2[c2]) );
    ulong len = c2 - c2fix;
    ulong beg = mat[0] + dlo_fast_chunk - len;

    while ( f2[c2] == f1[beg+len] ) { c2++; len++; }
    while ( c2fix > 0 && beg > 0 && f2[c2fix-1] == f1[beg-1] )
        { c2fix--; beg--; len++; }

    for( i=c3; i<c2fix; i++ )
    d.push_back( command(command::symbol,f2[i]) );
    d.push_back( command(command::block,beg,beg+len) );

    goto mainloop;

finalize:

    for( i=c3; i<size2; i++ )
    d.push_back( command(command::symbol,f2[i]) );
}
*/
// --------------------------------------------------------

bool read_int(istream & i, unsigned long & x)
{

    x = 0;
    char c;
    while (1)
    {
        i.get(c);
        if ( !i ) return false;
        if ( c == '\n' ) break;
        x *= 10;
        x += ( c - '0' );
    }
    return true;
}

bool read_string(istream & i, string & s)
{

    s = "";
    char c;
    while (1)
    {
        i.get(c);
        if ( !i ) return false;
        if ( c == '\n' ) break;
        s += c;
    }
    return true;
}
