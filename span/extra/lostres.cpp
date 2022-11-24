
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <set>
#include <map>

using namespace std;

string load(string txtname);
void save(string outname, const string & s);
set<string> load_words(string fname);


template <class T> string tos(const T & x) { ostringstream os; os << x; return os.str(); }

int main()
try
{
    ///cout << "breaking words ... " << flush;
    string txt = load("lostres.txt");
    save("los1.txt", txt);
    ///cout << "ok\n";

    set<string> igns = load_words("ignore.txt");

    const int MX = 10;

    {
        map<string, int> words;
        ifstream in("los1.txt");
        for ( string w; in >> w; )
        {
            if ( igns.find(w) != igns.end() ) continue;
            if ( words.find(w) == words.end() )
            {
                words[w] = 1;
                continue;
            }

            if ( words[w] == MX )
            {
                igns.insert(w);
                words.erase(w);
                cout << w << '\n';
                continue;
            }

            ++words[w];
        }
    }
}
catch ( string e)
{
    cout << "Error: " << e << '\n';
}


string translate(char c)
{
    typedef unsigned char uchar;
    uchar u = (uchar)c;

    string r;

    // words
    if ( u >= 'A' && u <= 'Z' ) return r + char(c - 'A' + 'a');
    if ( u >= 'a' && u <= 'z' ) return r + c;
    if ( u >= '0' && u <= '9' ) return r + c;
    if ( u == '\'' ) return r + c;

    if ( u == uchar(241) || u == uchar(225) || u == uchar(233) ) return r + c;
    if ( u == uchar(237) || u == uchar(243) || u == uchar(250) ) return r + c;
    if ( u == uchar(252) ) return r + c;
    if ( u == uchar(201) ) return r + char(233);
    if ( u == uchar(232) ) return r + char(233);
    if ( u == uchar(193) ) return r + char(225);
    if ( u == uchar(234) ) return r + char(233);
    if ( u == uchar(239) ) return r + 'i';

    if ( u == uchar(238) ) return r + char(237);
    if ( u == uchar(242) ) return r + 'o';
    if ( u == uchar(211) ) return r + 'o';
    if ( u == uchar(226) ) return r + 'a';
    if ( u == '_' ) return "";

    // text mistake
    if ( u == uchar(204) ) return r + 'l';
    if ( u == uchar(236) ) return r + 'i';
    if ( u == uchar(231) ) return r + 'c';
    if ( u == uchar(200) ) return r + 'e';
    if ( u == '=' ) return r + " - ";

    // space
    if ( u == ' ' || u == '\n' || u == '\r' ) return r + c;

    // punctuation
    if ( u == '.' || u == ',' || u == '-' ) return r + ' ' + c + ' ';
    if ( u == ':' || u == ';' || u == '!' ) return r + ' ' + c + ' ';
    if ( u == '(' || u == ')' || u == '?' ) return r + ' ' + c + ' ';
    if ( u == '*' || u == ')' || u == '?' ) return r + ' ' + c + ' ';
    if ( u == uchar(171) || u == uchar(187) ) return r + ' ' + c + ' ';
    if ( u == uchar(161) || u == uchar(191) ) return r + ' ' + c + ' ';

    if ( u == '"' ) return r + ' ' + c + ' ';

    throw string() + "bad char [" + c
    + "] (" + tos((int)(unsigned char)c) + ")";
}

string load(string txtname)
{
    string r;

    ifstream in(txtname, ios::binary);

    while (in)
    {
        char c;
        in.get(c);
        if ( !in ) break;

        string t = translate(c);

        r += t;
    }

    return r;
}

void save(string outname, const string & s)
{
    ofstream of(outname, ios::binary);
    of << s;
}

set<string> load_words(string fname)
{
    set<string> r;
    ifstream in(fname);

    for ( string w; in >> w; ) r.insert(w);
    return r;
}

