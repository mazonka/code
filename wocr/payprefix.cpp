#include "ccrun.h"
#include "ccrut.h"

#include <regex>

void cmain()
{
    auto ents = fsut::readdir();

    for ( auto f : ents.files().names() )
    {
        if ( !ol::endsWith(f, ".pdf") && !ol::endsWith(f, ".PDF") )
            continue;
        cout << f << '\n';

        string text = ol::file2str(f);
        cout << "size " << text.size() << '\n';

        const auto & npos = string::npos;
        auto idate = text.find("ate:");
        if ( idate == npos ) continue;

        cout << "idate=" << idate << '\n';

        string date;
        {
            std::smatch sm;
            //std::regex e ("\\d{2}[-/. ]\\d{2}[-/. ]\\d{4}");
            std::regex e(R"(\(\d{2}/\d{2}/20\d{2}\))");
            std::regex_search (text, sm, e);

            if ( sm.size() != 1 )
            {
                cout << "Date not found, matches: " << (int)sm.size() << '\n';
                if ( sm.size() < 1 ) continue;
                continue;
            }

            string r = sm[0];
            cout << "found " << r << '\n';
            date = r.substr(9, 2);
            date += r.substr(4, 2);
            date += r.substr(1, 2);
        }
        cout << "date = " << date << '\n';


        string amount;
        {
            std::smatch sm;
            std::regex e(R"(\(\$\d{1,4}\.\d{2}\))");
            std::regex_search (text, sm, e);

            if ( sm.size() != 1 )
            {
                if ( sm.size() < 1 ) // try $1,234.56
                {
                    std::smatch sm2;
                    std::regex e2(R"(\(\$\d\,\d\d\d\.\d{2}\))");
                    std::regex_search (text, sm2, e2);

                    if ( sm2.size() == 1 )
                    {
                        string r = sm2[0];
                        cout << "found2 " << r << '\n';
                        auto ipoint = r.find(".");
                        if ( ipoint == npos ) throw "Bad match";
                        amount = r.substr(2, 1);
                        amount += r.substr(4, ipoint - 4);
                        amount += r.substr(ipoint + 1, 2);
                        while (amount.size() < 6 ) amount = "0" + amount;
                    }
                }
                else
                {
                    cout << "Amount not found, matches: " << (int)sm.size() << '\n';
                    continue;
                }
            }
            else
            {
                string r = sm[0];
                cout << "found " << r << '\n';
                auto ipoint = r.find(".");
                if ( ipoint == npos ) throw "Bad match";
                amount = r.substr(2, ipoint - 2);
                amount += r.substr(ipoint + 1, 2);
                while (amount.size() < 6 ) amount = "0" + amount;
            }
        }
        cout << "amount = " << amount << '\n';


        auto fn = date + "_" + amount + "_";
        if ( f.find(fn) != npos )
        {
            cout << "[" << f << "]\n";
            continue;
        }

        fn += f;
        fs::rename(f, fn);
        cout << "[" << f << "] -> [" << fn << "]\n";
    }
}

