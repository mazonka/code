#include <set>

#include "ccrun.h"
#include "ccrut.h"

string ffmpeg = "c:\\app\\FormatFactory\\ffmpeg -i %1 -i %2 -c copy %3.mkv";

void cmain()
{
    auto files = fsut::readdir();
    cout << "files # = " << files.size() << "\n";

    std::map<string, ol::ivec<fs::path>> base2path;
    for ( auto fe : files.files() )
    {
        fs::path fp(fe.first);
        string ext = fp.extension().string();
        if ( ext == ".vtt" ) continue;
        if ( ext == ".json" ) continue;

        string basename = fp.stem().stem().stem().string();
        cout << basename << '\n';
        base2path[basename] += fp;
    }

    for ( const auto & e : base2path )
    {
        const auto & v = e.second;
        if ( v.size() != 2 ) continue;

        string basename = e.first;
        cout << "\nbasename: " << basename << "\n";
        std::set<string> exts;
        for ( auto p : e.second )
        {
            string ext = p.extension().string();
            cout << p.string() << " " << ext << '\n';
            exts.insert(ext);
        }

        if ( exts.size() != 2 ) never;
        auto it = exts.begin();
        if ( *it != ".m4a" ) continue;
        if ( *++it != ".mp4" ) continue;

        string cmd = ffmpeg;
        ol::replaceAll(cmd, "%3", basename);
        ol::replaceAll(cmd, "%1", v[0].string());
        ol::replaceAll(cmd, "%2", v[1].string());
        cout << cmd << '\n';
        sys(cmd);
    }
}
