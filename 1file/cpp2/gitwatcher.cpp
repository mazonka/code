
#include <thread>

using namespace std::chrono_literals;

#include "ccrun.h"
#include "ccrut.h"


const char * gitDirs[] = 
{ 
 "mazonka/code", "mazonka/w23", "mazonka/g23", 
 "mazonka/g22", "momalab/simfhe"
};

fs::path desktop = "C:/Users/oleg/Desktop";
int iicon[] =
{
    76, 0, 0, 0, 1, 20, 2, 0, 0, 0, 0, 0, 192, 0, 0, 0
    , 0, 0, 0, 70, 203, 0, 8, 0, 17, 0, 0, 0, 244, 185, 133, 136
    , 181, 55, 217, 1, 251, 159, 214, 22, 171, 7, 218, 1, 148, 165, 207, 22
    , 171, 7, 218, 1, 0, 48, 0, 0, 229, 0, 0, 0, 1, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 0, 20, 0
    , 31, 80, 224, 79, 208, 32, 234, 58, 105, 16, 162, 216, 8, 0, 43, 48
    , 48, 157, 58, 0, 46, 128, 58, 204, 191, 180, 44, 219, 76, 66, 176, 41
    , 127, 233, 154, 135, 198, 65, 38, 0, 1, 0, 38, 0, 239, 190, 17, 0
    , 0, 0, 244, 185, 133, 136, 181, 55, 217, 1, 65, 162, 167, 4, 171, 7
    , 218, 1, 183, 189, 169, 4, 171, 7, 218, 1, 20, 0, 0, 0, 75, 0
    , 0, 0, 28, 0, 0, 0, 1, 0, 0, 0, 28, 0, 0, 0, 52, 0
    , 0, 0, 0, 0, 0, 0, 74, 0, 0, 0, 24, 0, 0, 0, 3, 0
    , 0, 0, 166, 114, 170, 112, 16, 0, 0, 0, 87, 105, 110, 100, 111, 119
    , 115, 0, 67, 58, 92, 85, 115, 101, 114, 115, 92, 111, 108, 101, 103, 92
    , 68, 101, 115, 107, 116, 111, 112, 0, 0, 1, 0, 46, 0, 34, 0, 37
    , 0, 83, 0, 121, 0, 115, 0, 116, 0, 101, 0, 109, 0, 82, 0, 111
    , 0, 111, 0, 116, 0, 37, 0, 92, 0, 115, 0, 121, 0, 115, 0, 116
    , 0, 101, 0, 109, 0, 51, 0, 50, 0, 92, 0, 105, 0, 109, 0, 97
    , 0, 103, 0, 101, 0, 114, 0, 101, 0, 115, 0, 46, 0, 100, 0, 108
    , 0, 108, 0, 96, 0, 0, 0, 3, 0, 0, 160, 88, 0, 0, 0, 0
    , 0, 0, 0, 97, 100, 117, 97, 101, 100, 49, 49, 53, 49, 51, 108, 112
    , 119, 120, 0, 78, 249, 197, 25, 217, 193, 88, 70, 167, 75, 80, 27, 55
    , 90, 133, 23, 106, 17, 242, 59, 169, 163, 237, 17, 130, 149, 80, 118, 175
    , 44, 238, 117, 78, 249, 197, 25, 217, 193, 88, 70, 167, 75, 80, 27, 55
    , 90, 133, 23, 106, 17, 242, 59, 169, 163, 237, 17, 130, 149, 80, 118, 175
    , 44, 238, 117, 127, 1, 0, 0, 9, 0, 0, 160, 77, 0, 0, 0, 49
    , 83, 80, 83, 237, 48, 189, 218, 67, 0, 137, 71, 167, 248, 208, 19, 164
    , 115, 102, 34, 49, 0, 0, 0, 100, 0, 0, 0, 0, 31, 0, 0, 0
    , 16, 0, 0, 0, 111, 0, 108, 0, 101, 0, 103, 0, 32, 0, 40, 0
    , 67, 0, 58, 0, 92, 0, 85, 0, 115, 0, 101, 0, 114, 0, 115, 0
    , 41, 0, 0, 0, 0, 0, 0, 0, 148, 0, 0, 0, 49, 83, 80, 83
    , 48, 241, 37, 183, 239, 71, 26, 16, 165, 241, 2, 96, 140, 158, 235, 172
    , 33, 0, 0, 0, 10, 0, 0, 0, 0, 31, 0, 0, 0, 8, 0, 0
    , 0, 68, 0, 101, 0, 115, 0, 107, 0, 116, 0, 111, 0, 112, 0, 0
    , 0, 21, 0, 0, 0, 15, 0, 0, 0, 0, 64, 0, 0, 0, 0, 221
    , 133, 137, 181, 55, 217, 1, 45, 0, 0, 0, 4, 0, 0, 0, 0, 31
    , 0, 0, 0, 14, 0, 0, 0, 83, 0, 121, 0, 115, 0, 116, 0, 101
    , 0, 109, 0, 32, 0, 70, 0, 111, 0, 108, 0, 100, 0, 101, 0, 114
    , 0, 0, 0, 21, 0, 0, 0, 14, 0, 0, 0, 0, 64, 0, 0, 0
    , 65, 162, 167, 4, 171, 7, 218, 1, 0, 0, 0, 0, 89, 0, 0, 0
    , 49, 83, 80, 83, 166, 106, 99, 40, 61, 149, 210, 17, 181, 214, 0, 192
    , 79, 217, 24, 208, 61, 0, 0, 0, 30, 0, 0, 0, 0, 31, 0, 0
    , 0, 22, 0, 0, 0, 67, 0, 58, 0, 92, 0, 85, 0, 115, 0, 101
    , 0, 114, 0, 115, 0, 92, 0, 111, 0, 108, 0, 101, 0, 103, 0, 92
    , 0, 68, 0, 101, 0, 115, 0, 107, 0, 116, 0, 111, 0, 112, 0, 0
    , 0, 0, 0, 0, 0, 57, 0, 0, 0, 49, 83, 80, 83, 177, 22, 109
    , 68, 173, 141, 112, 72, 167, 72, 64, 46, 164, 61, 120, 140, 29, 0, 0
    , 0, 104, 0, 0, 0, 0, 72, 0, 0, 0, 97, 194, 74, 74, 132, 104
    , 157, 77, 149, 61, 248, 211, 57, 242, 238, 69, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0
};
string icon;

void name(string &n)
{
    ol::replaceAll(n, "/", "_");
    ol::replaceAll(n, "mazonka", "maz");
    ol::replaceAll(n, "momalab", "lab");
    n+=".lnk";
}

void cleanSignal(string gdir)
{
    ///ol::replaceAll(gdir, "/", "_"); gdir+=".lnk";
    name(gdir);
    if ( !fs::exists(desktop / gdir) ) return;
    fs::remove(desktop / gdir);
}

void setSignal(string gdir)
{
    ///ol::replaceAll(gdir, "/", "_"); gdir+=".lnk";
    name(gdir);
    std::ofstream(desktop / gdir) << icon;
}

void cmain()
{
    for ( int i : iicon ) icon += char(i);
    //std::ofstream("z.lnk") << icon;

    for (int c = 0; c < 1 + 1 * (120 * 60 * 24 * 30); c++)
    {
        std::this_thread::sleep_for(500ms);
        cout << (c % 2 ? "o" : "O") << '\r';
        if ( c % (60 * 1) ) continue;
        ///cout << "Ahahah\n";
        for ( auto gdir : gitDirs )
        {
            cout << "Check " << gdir << std::flush;
            fsut::Pushd dummy(gdir);
            if ( !dummy ) throw "Cannot enter " + string(gdir);
            sys("git status . > gitwatcher.tmp");
            string git_reply = ol::file2str("gitwatcher.tmp");
            fs::remove("gitwatcher.tmp");
            auto clean = git_reply.find("Changes not staged for commit");
            if ( clean == string::npos )
            {
                cout << " - ok\n";
                cleanSignal(gdir);
                continue;
            }
            cout << " - need commit\n";
            setSignal(gdir);
        }
    }
}