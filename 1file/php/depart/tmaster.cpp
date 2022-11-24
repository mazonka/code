#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <regex>
#include <vector>
#include <exception>
#include <cctype>

using std::cout;
using std::string;

const bool DB = false;
const bool OUT = false;
const bool UseRegex = false;

const char * sentFileName = "sent.txt";
//string email = "Oleg.Mazonka@dsto.defence.gov.au,Julia.Piotto@dsto.defence.gov.au";
string email = "JOADEDN-23Labs@dsto.defence.gov.au";
const char * wgetcmd = "wget http://last2leave23.dsto.defence.gov.au/index.php?wget=z -O z -o log.tmp";

struct Ex { string m; int i; };

int main()
try
{
    // cout<<"hello\n";
    // 1 get info: Date, Who, Time
    // 2 load sent and compare
    // 3 if different save and send email

    string prepareText();
    string s = prepareText();

    string loadSent();
    string o = loadSent();

    if ( s == o )
    {
        if(OUT) cout << "No new data\n";
        std::ofstream of("no_update.log");
        of<<o<<'\n';
        return 0;
    }

    void saveSent(string);
    saveSent(s);

    void sendMail(string);
    sendMail(s);
}
catch (Ex e)
{
    cout << "Error: " << e.m << " (" << e.i << ")" << "\n";
    std::ofstream of("error.log");
    of << "Error: " << e.m << " (" << e.i << ")" << "\n";
    return 3;
}
catch (const std::regex_error & e)
{
    cout << "regex_error caught: " << e.what() << '\n';
    cout << e.code() << '\n';
    std::ofstream of("error.log");
    of << "regex_error caught: " << e.what() << '\n';
    return 2;
}
catch (std::exception & e)
{
    cout << "Std exception: " << e.what() << "\n";
    std::ofstream of("error.log");
    of << "Std exception: " << e.what() << "\n";
    return 2;
}
catch (...)
{
    cout << "Exception\n";
    std::ofstream of("error.log");
    of << "Exception\n";
    return 1;
}

string loadFile(string fn)
{
    std::ifstream in(fn.c_str());
    if( !in ) throw Ex { "Cannot open "+fn, 0 };
    string r;

    while (1)
    {
        char c;
        in.get(c);
        if ( !in ) break;
        r += c;
    }

    return r;
}

string loadSent()
{
    return loadFile(sentFileName);
}

void saveSent(string f)
{
    std::ofstream of(sentFileName);
    of << f;
}

void sendMail(string t)
{
    string cmd = "/usr/sbin/sendmail " + email + " < " + sentFileName;
    if(OUT) cout << cmd << '\n';
    if (!DB) std::system(cmd.c_str());
}

struct Rec
{
    string name;
    string hime;
    int val;
};
using Vrec = std::vector<Rec>;

string prepareText()
{
    if (!DB) { std::ofstream of("z"); }
    if (!DB) std::system(wgetcmd);
    string s = loadFile("z");

    string parseDate(string);
    string date = parseDate(s);

    string parseTime(string);
    string hime = parseTime(s);

    Vrec parseNames(string);
    Vrec vr = parseNames(s);

    Vrec findNames(const Vrec &, string);
    Vrec vn = findNames(vr, hime);

    string r = "Subject: Departure time for today: " + hime + "\n\n";
    r += "The final departure time for today, " + date + " is: " + hime + "\n\n";

    for ( auto x : vn )
        r += x.name + " (" + x.hime + ")\n";

    r += "\n\nhttp://last2leave23.dsto.defence.gov.au";
    r += "\nSent to: "+email+"\n";

    return r;
}

string parseDate(string s)
{
    string r;
    if (UseRegex)
    {
        std::smatch sm;

        std::regex e ("\\d{2}[-/. ]\\d{2}[-/. ]\\d{4}");
        std::regex_search (s, sm, e);

        if ( sm.size() != 1 )
            throw Ex {"Date not found, matches: ", (int)sm.size()};

        r = sm[0];
    }
    else
    {
        for ( size_t i = 0; i < s.size() - 12; i++ )
            if ( std::isdigit(s[i]) && std::isdigit(s[i + 1])
                    && s[i + 2] == '/' && std::isdigit(s[i + 3]) && std::isdigit(s[i + 4]) )
            {
                r = s.substr(i, 10);
                break;
            }
    }

    if(OUT) cout << "Date: " << r << "\n";

    return r;
}

string cutmeout(string s, string from, string to)
{
    size_t i = s.find(from);
    size_t j = s.find(to);
    if ( i == string::npos || j == string::npos ) throw Ex {"Bad html", 0};
    s = s.substr(i+from.size(), j - i -from.size());
    return s;
}

string parseTime(string s)
{
    s = cutmeout(s,"<table","</table");

    string hime;
    if (UseRegex)
    {

        std::smatch sm;

        std::regex e (">\\d{4}<");
        std::regex_search (s, sm, e);

        if ( sm.size() != 1 )
            throw Ex {"Time not found, matches: ", (int)sm.size()};

        hime = string(sm[0]).substr(1, 4);

    }
    else
    {
	    s = cutmeout(s,"<font","/font>");
	    hime = cutmeout(s,">","<");
    }

    if(OUT) cout << "Time: " << hime << "\n";

    return hime;
}

string cutremain(string s, string to)
{
    s = s.substr(s.find(to)+to.size());
    return s;
}

Vrec parseNames(string s)
{
    s = cutremain(s,"/table>");
    s = cutmeout(s,"<table","/table>");

    Vrec r;

	while( s.find("<tr>") != string::npos )
	{
		string line = cutmeout(s,"<tr","/tr>");
		string name = cutmeout(s,"<td","/td>");
		string tim = cutmeout(cutremain(s,"/td>"),"<td","/td>");
		name = cutmeout(name,">","<");
		tim = cutmeout(tim,">","<");
		s = cutremain(s,"/tr>");

        int i = 0;
	if(tim.size() && std::isdigit(tim[0]) ) i = std::atoi(tim.c_str());
        if (DB) cout << "[" << name << "]\t[" << tim << "]" << " " << i << "\n";
	if( name.empty() || tim.empty() ) continue;

        r.push_back(Rec {name, tim, i});
	}


    return r;
}

Vrec findNames(const Vrec & vr, string hime)
{
    int t = std::atoi(hime.c_str());

    Vrec r;

    for ( auto x : vr )
    {
        if ( x.val >= t )
            r.push_back(x);
    }

    return r;
}

