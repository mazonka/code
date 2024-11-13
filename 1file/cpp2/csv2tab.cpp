// this removes trailing commas and spaces,
// removes many spaces to one
// inserts 'x' in empty fields, and
// replaces commas with tabs

// unix command to revert order of lines is 'tac'

#include "ccrun.h"
#include "ccrut.h"

void cmain()
{
    if ( args.size() != 2 )
    {
        cout << "use in_file out_file\n";
        return;
    }


    std::ifstream in(args[0]);
    std::ofstream out(args[1]);

    while (in)
    {
        string line;
        getline(in, line);
        if ( !in ) break;

        while (1)
        {
            if ( line.empty() ) break;
            if ( !ol::endsWith(line, " ")
                    && !ol::endsWith(line, ",") ) break;
            line = line.substr(0, line.size() - 1);
        }

        ol::replaceAll(line, "  ", " ");
        ol::replaceAll(line, ",,", ",x,");
        ol::replaceAll(line, ",", "\t");

        out << line << '\n';
    }

}

