#include <iostream>
#include <fstream>
#include <sstream>
#include <limits.h>
#include <string>

using namespace std;

int rez_rep(int ac, char * av[]);
int rez_xtr(int ac, char * av[]);
int rez_sub(int ac, char * av[]);

extern const char * help[];

int main(int ac, char * av[])
{

    if ( ac < 2 )
    {
        cerr << help[0];
        return 1;
    }

    if ( string("help") == string(av[1]) )
    {

        if ( ac == 3)
        {

            if ( string("report") == string(av[2]) )
                cout << help[1];

            else if ( string("extract") == string(av[2]) )
                cout << help[2];

            else if ( string("subst") == string(av[2]) )
                cout << help[3];

            else
                cout << help[0];

        }
        else

            cout << help[0];

        return 0;

    }

    if ( string("report") == string(av[1]) )
        return rez_rep(ac - 1, av + 1);

    if ( string("extract") == string(av[1]) )
        return rez_xtr(ac - 1, av + 1);

    if ( string("subst") == string(av[1]) )
        return rez_sub(ac - 1, av + 1);

    cout << help[0];
    return 0;
}

int rez_rep(int ac, char * av[])
{

    if ( ac < 3 )
    {
        cerr << "Use: report <filename> block_size\n"
             "e.g. report a.exe 10000\n";
        return 1;
    }

    ifstream inp(av[1], ios::binary);

    if (!inp)
    {
        cerr << "Cannot open " << av[1];
        return 2;
    }

    unsigned long block;

    {
        stringstream s(av[2]);
        s >> block;
        if ( block < 1 || block > INT_MAX )
        {
            cerr << "Block_size must be > 0 and < "
                 << INT_MAX << ", it is " << block << '\n';
            return 3;
        }
    }

    string rep_name(av[1]);
    rep_name += string(".report.txt");

    ofstream out(rep_name.c_str());

    if ( !out )
    {
        cerr << "Cannot open " << rep_name.c_str() << endl;
        return 4;
    }

    cout << av[1] << " " << block << endl;
    out << av[1] << " " << block << endl;

    char ch;
    unsigned long num = 0, i = 0, sum = 0, total = 0;

    while ( inp.get(ch) )
    {

        sum += (unsigned char)ch;
        if ( ++i == block )
        {

            out << ++num << " " << hex << sum << dec << endl;
            cout << num << " ";
            total += sum;
            sum = 0;
            i = 0;

        }

    }

    if (i)
        out << ++num << " " << hex << sum << dec << endl;

    out << "0 " << i << endl;
    cout << "Total: " << hex << total + sum << endl;

    return 0;
}

int rez_xtr(int ac, char * av[])
{

    if ( ac < 4 )
    {
        cerr << "Use: extract <filename> block_size block_number\n"
             "e.g. extract a.exe 100 1234\n";
        return 1;
    }

    ifstream inp(av[1], ios::binary);

    if (!inp)
    {
        cerr << "Cannot open " << av[1];
        return 2;
    }

    unsigned long block;

    {
        stringstream s(av[2]);
        s >> block;
        if ( block < 1 || block > INT_MAX )
        {
            cerr << "Block_size must be > 0 and < "
                 << INT_MAX << ", it is " << block << '\n';
            return 3;
        }
    }

    unsigned long nblock = 0;

    {
        stringstream s(av[3]);
        s >> nblock;
        if ( nblock < 1 || nblock > INT_MAX )
        {
            cerr << "Block_number must be > 0 and < "
                 << INT_MAX << ", it is " << nblock << '\n';
            return 5;
        }
    }

    string block_name(av[1]);
    block_name += string(".") + av[2] + "_" + av[3] + ".rez";

    ofstream out(block_name.c_str(), ios::binary);

    if ( !out )
    {
        cerr << "Cannot open " << block_name.c_str() << endl;
        return 4;
    }

    cout << "Writing " << block_name.c_str() << endl;

    char ch;
    unsigned long i = 0;

    inp.seekg((nblock - 1)*block);

    while ( inp.get(ch) && i++ < block )
        out << ch;

    return 0;
}


int rez_sub(int ac, char * av[])
{

    if ( ac < 4 )
    {
        cerr << "Use: subst <filename> block_size block_number\n"
             "e.g. subst a.exe 100 1234\n";
        return 1;
    }

    ifstream inp(av[1], ios::binary);

    if (!inp)
    {
        cerr << "Cannot open " << av[1];
        return 2;
    }

    unsigned long block;

    {
        stringstream s(av[2]);
        s >> block;
        if ( block < 1 || block > INT_MAX )
        {
            cerr << "Block_size must be > 0 and < "
                 << INT_MAX << ", it is " << block << '\n';
            return 3;
        }
    }

    unsigned long nblock = 0;

    {
        stringstream s(av[3]);
        s >> nblock;
        if ( nblock < 1 || nblock > INT_MAX )
        {
            cerr << "Block_number must be > 0 and < "
                 << INT_MAX << ", it is " << nblock << '\n';
            return 5;
        }
    }

    string block_name(av[1]), out_name(av[1]);
    block_name += string(".") + av[2] + "_" + av[3] + ".rez";
    out_name += string(".subst");

    ifstream bin(block_name.c_str(), ios::binary);
    ofstream out(out_name.c_str(), ios::binary);

    if ( !out )
    {
        cerr << "Cannot open " << out_name.c_str() << endl;
        return 4;
    }
    if ( !bin )
    {
        cerr << "Cannot open " << block_name.c_str() << endl;
        return 6;
    }

    cout << "Copying... Looking for block " << nblock << endl;

    char ch;
    unsigned long num = 0, i = 0;
    bool write = false;

    while ( inp.get(ch) )
    {

        if ( !(i++ % block) && (i = 1) )
            if ( ++num == nblock )
            {
                cout << "Writing " << block_name.c_str() << endl;
                write = true;
            }
            else
                write = false;

        if ( write && bin )
            bin.get(ch);

        out << ch;

    }

    return 0;
}


//------------------------------------------------------------

const char * help[4] =
{

    "Use: \n"
    "rezalka help\n"
    "rezalka help report\n"
    "rezalka help extract\n"
    "rezalka help subst\n"
    "rezalka report\n"
    "rezalka extract\n"
    "rezalka subst\n"
    ,

    "This function prints control sums.\n"
    "Example:\n"
    "we have a file 'ten' of size 10:\n"
    "	abcd1234zz\n"
    "Running \n"
    "	rezalka report ten 4\n"
    "produces\n"
    "	ten 4\n"
    "	1 18a\n"
    "	2 ca\n"
    "	3 f4\n"
    "	0 2\n"
    "which means\n"
    "ten - filename \n"
    "4 - size of the block\n"
    "1 18a - 1st block has a sum 18a\n"
    "2 ca - 2nd block\n"
    "3 f4 - 3rd block\n"
    "0 means that 3rd block was the last one\n"
    "2 is the size of the 3rd (last) block\n"
    "\n"
    "So size of a file is exactly \n"
    "Number_of_the_last_block * size_of_the_block \n"
    "+ size_of_the_last_block\n"
    "\n"
    "In this case 3*4+2 = 10\n"
    "\n"

    ,

    "rezalka extract extracts blocks from a file\n",

    "rezalka subst substitutes blocks from a file\n"
};
