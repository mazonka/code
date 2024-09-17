#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <memory>

#include "olu.h"
#include "gfu.h"
#include "hash.h"
#include "jadd.h"

using std::cout;

string g_vlt_name = ".gf.vlt";

inline static bool isVltFile(const jadd::File & f)
{
    return f.pth.filename().string() == g_vlt_name;
}

struct VltFile
{
    fs::path fullName;
    using Entry = jadd::File;
    ivec<Entry> entries;
    jadd::File data;

    static VltFile load(fs::path dir);
    Entry genEntry() const;
    static Entry genFileEntry(const jadd::File & f);
    static string entry2str(const Entry & f);
    VltFile(fs::path dir);
    void save() const;
    void operator+=(Entry entry);
};

void vault_build();
void vault_top();

int main_vault(ivec<string> args)
{
    if ( args.empty() )
    {
        cout << "hashing files\n";
        cout << "vault module creates hashes of files and directories\n";
        cout << "usage: vault {build|update|check|deep|same|clean|top}\n";
        cout << "build - rebuild gf hashfiles, overwrite\n";
        cout << "update - check, and if necessary, update hashfiles\n";
        cout << "check - verify that no changes in file tree\n";
        cout << "deep - validate hashes\n";
        cout << "same [dir1] ... - list same files/trees in size order\n";
        cout << "clean - remove [" << g_vlt_name << "] files\n";
        cout << "top - show top level hash\n";
        return 0;
    }

    if ( args.size() < 1 ) throw "Bad arguments";

    string cmd = args[0];

    ///cout << "AAA " << cmd << '\n';


    if (0) {}
    else if (cmd == "top") vault_top();
    else if (cmd == "build") vault_build();
    else if (cmd == "update") never;
    else if (cmd == "check") never;
    else if (cmd == "deep") never;
    else if (cmd == "clean") never;
    else if (cmd == "same")
    {
        never;
    }
    else throw "Command " + cmd + " not valid";

    return 0;
}

int vault_buildR_cntr;
void vault_buildR(jadd::Files & tFiles, jadd::DirNode * dir)
{
    if (0)
    {
        cout << "DirNode: dirName [" << dir->dirName << "]";
        cout << " fullName [" << dir->fullName << "]";
        cout << " dirs sz [" << dir->dirs.size() << "]";
        cout << " idxs sz [" << dir->idxs.size() << "]";
        cout << " hash [" << dir->hash << "]";
        cout << " sz [" << dir->sz << "]\n";
    }

    for (auto * d : dir->dirs) vault_buildR(tFiles, d);

    ivec<jadd::File> files;
    for (int fileidx : dir->idxs)
    {
        jadd::File file = tFiles.files[fileidx];
        jadd::File::fillHash("", file, false);
        //cout << "File: "; file.print();
        if ( !isVltFile(file) ) files += file;
        cout << (++vault_buildR_cntr) << '\r';
    }

    VltFile vltFile(dir->fullName);

    // first list all dirs
    for (auto * d : dir->dirs)
    {
        VltFile dVltFile = VltFile::load(d->fullName);
        vltFile += dVltFile.genEntry();
    }

    for (auto f : files)
    {
        vltFile += VltFile::genFileEntry(f);
    }

    vltFile.save();
}

void vault_build()
{
    jadd::Files tFiles;
    jadd::DirNode dirTree;
    tFiles.dirTree = &dirTree;
    jadd::loadFrom(".", tFiles);
    cout << "read " << tFiles.files.size() << " files\n";

    vault_buildR_cntr = 0;
    jadd::DirNode * dir = tFiles.dirTree;
    vault_buildR(tFiles, dir);
    cout << '\n';
}

void vault_top()
{
    VltFile vlt = VltFile::load("");
    jadd::File f = vlt.genEntry();
    if (f.sz == 0)
        cout << "Vault is empty\n";
    else
        cout << f.hashFile << '\n';
}

VltFile VltFile::load(fs::path dir)
{
    VltFile file(dir);

    {
        //std::ifstream in(file.fullName);
    }
    string svlt = ol::file2str(file.fullName);
    std::istringstream in(svlt);

    {
        file.data.tc = 0;
        file.data.sz = svlt.size();
        file.data.pth = dir.filename();
        file.data.hashFile = ha::hashHex(svlt);

        if ( file.data.sz <= jadd::HEADSZ )
            file.data.hashHead = file.data.hashFile;
        else
        {
            string head = svlt.substr(0, jadd::HEADSZ);
            file.data.hashHead = ha::hashHex(head);
        }
    }

    auto readEnt = [&in]() -> Entry
    {
        Entry e;
        in >> e.sz;
        if (!in) return e;
        in >> e.tc;
        in >> e.hashHead;
        in >> e.hashFile;
        if (e.hashFile == "=") e.hashFile = e.hashHead;
        string name;
        std::getline(in >> std::ws, name);
        e.pth = name;
        if (!in) e.pth = fs::path();
        return e;
    };

    unsigned long long dirsz = 0;
    while (1)
    {
        Entry e = readEnt();
        if (e.pth.empty()) break;
        file += e;
        dirsz += e.sz;
    }

    file.data.sz += dirsz;
    return file;
}

VltFile::Entry VltFile::genEntry() const
{
    return data;
}

VltFile::Entry VltFile::genFileEntry(const jadd::File & f)
{
    return Entry(f);
}

string VltFile::entry2str(const Entry & f)
{
    std::ostringstream os;
    os << f.sz << ' ' << f.tc << '\n';
    if (f.hashFile.empty()) never;
    if (f.hashHead.empty()) never;
    os << f.hashHead << '\n';
    if (f.hashHead == f.hashFile) os << "=\n";
    else os << f.hashFile << '\n';
    string fname = f.pth.filename().string();
    os << fname << '\n';
    return os.str();
}

VltFile::VltFile(fs::path dir)
{
    fullName = dir / g_vlt_name;
}

void VltFile::save() const
{
    std::ofstream of(fullName, std::ios::binary);

    for (auto e : entries)
        of << entry2str(e) << '\n';
}

void VltFile::operator+=(Entry entry)
{
    entries += entry;
}
