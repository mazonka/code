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

inline static string fname(const jadd::File & f) { return f.pth.filename().string(); }
inline static bool isVltFile(const jadd::File & f) { return fname(f) == g_vlt_name; }

struct VltFile
{
    fs::path fullName;
    using Entry = jadd::File;
    ivec<Entry> entries;
    jadd::File data;

    static VltFile load(fs::path dir);
    string calcHash() const;
    Entry genEntry() const;
    static Entry genFileEntry(const jadd::File & f);
    static string entry2str(const Entry & f);
    VltFile(fs::path dir);
    void save() const;
    void operator+=(Entry entry);
    bool update(jadd::File & f) const;
    Entry findByName(string s) const;
    void setDataHash() { data.hashFile = calcHash(); }
};



void vault_build();
void vault_top();
void vault_clean();
void vault_update();





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
    else if (cmd == "build") vault_build();
    else if (cmd == "top") vault_top();
    else if (cmd == "clean") vault_clean();
    else if (cmd == "update") vault_update();
    else if (cmd == "check") never;
    else if (cmd == "deep") never;
    else if (cmd == "same")
    {
        never;
    }
    else throw "Command " + cmd + " not valid";

    return 0;
}

VltFile VltFile::load(fs::path dir)
{
    VltFile vfile(dir);

    {
        //std::ifstream in(file.fullName);
    }
    string svlt = ol::file2str(vfile.fullName);
    std::istringstream in(svlt);

    {
        vfile.data.tc = 0;
        vfile.data.sz = svlt.size();
        vfile.data.pth = dir.filename();
        //file.data.hashFile = ha::hashHex(svlt);
        vfile.data.hashHead = "@";
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
    ///std::set<string> set_filehashes;
    while (1)
    {
        Entry e = readEnt();
        if (e.pth.empty()) break;
        vfile += e;
        dirsz += e.sz;
        ///set_filehashes.insert(e.hashFile);
    }

    ///string filehashes;
    ///for (auto s : set_filehashes) filehashes += s;
    ///file.data.hashFile = ha::hashHex(filehashes);
    vfile.setDataHash();
    vfile.data.sz += dirsz;
    return vfile;
}

string VltFile::calcHash() const
{
    std::set<string> set_filehashes;
    for ( auto e : entries )
    {
        set_filehashes.insert(e.hashFile);
    }

    string filehashes;
    for (auto s : set_filehashes) filehashes += s;
    return ha::hashHex(filehashes);
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

bool VltFile::update(jadd::File & f) const
{
    // update File from vlt
    // if vlt ok, copy hashes
    // if not, reread hashes

    Entry e = findByName(fname(f));
    if (fname(e).empty()) goto no;
    if (e.tc != f.tc) goto no;
    if (e.sz != f.sz) goto no;
    if ( fname(e) != fname(f) ) throw f.pth.string() + " vlt file corrupted";

    f.hashFile = e.hashFile;
    f.hashHead = e.hashHead;

    return true;

no:
    jadd::File::fillHash("", f, false);
    return false;
}

VltFile::Entry VltFile::findByName(string s) const
{
    for (auto f : entries)
        if (fname(f) == s) return f;
    return Entry();
}

static int vault_buildR_cntr;
static void vault_buildR(jadd::Files & tFiles, jadd::DirNode * dir)
{
    if (0) dir->print();

    for (auto * d : dir->dirs) vault_buildR(tFiles, d);

    ivec<jadd::File> files;
    for (int fileidx : dir->idxs)
    {
        jadd::File file = tFiles.files[fileidx];
        jadd::File::fillHash("", file, false);
        //cout << "File: "; file.print();
        if (!isVltFile(file)) files += file;
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

void vault_clean()
{
    jadd::Files tFiles;
    jadd::loadFrom(".", tFiles);
    auto sz = tFiles.files.size();
    cout << "read " << sz << " files\n";

    ivec<string> failed;
    for (auto f : tFiles.files)
    {
        if (!isVltFile(f)) continue;
        auto s = f.pth.string();
        if (!ol::delfile(s)) failed += s;
        cout << f.pth.string() << '\n';
    }

    if (!failed.empty())
    {
        cout << "\nFAILED to remove: \n";
        for (auto s : failed)
            cout << s << '\n';
        cout << "are listed files failed to remove\n";
    }
}


static int vault_updateR_cntr;
static int vault_updateR(jadd::Files & tFiles, jadd::DirNode * dir)
{
    if (0) dir->print();

    int updated = 0;
    ivec<jadd::DirNode *> dirs_same, dirs_chgd;
    for (auto * d : dir->dirs)
    {
        int c = vault_updateR(tFiles, d);
        updated += c;
        if (c == 0) dirs_same += d;
        else dirs_chgd += d;
    }

    VltFile vltFileNow = VltFile::load(dir->fullName);

    int files_same = 0, files_chgd = 0;
    ivec<jadd::File> files;
    for (int fileidx : dir->idxs)
    {
        cout << "[slow] " << (++vault_updateR_cntr) << '\r';
        jadd::File file = tFiles.files[fileidx];
        if (isVltFile(file)) continue;

        bool ok = vltFileNow.update(file);

        if (ok) ++files_same;
        else ++files_chgd;
        files += file;
    }

    int ent_same = files_same + dirs_same.size();
    if ( (ent_same == vltFileNow.entries.size())
            && ( files_chgd == 0 )
            && dirs_chgd.empty() )
        return updated;

    // update required

    VltFile vltFileNew(dir->fullName);

    for (auto * d : dir->dirs)
    {
        VltFile dVltFile = VltFile::load(d->fullName);
        vltFileNew += dVltFile.genEntry();
    }

    for (auto f : files)
    {
        vltFileNew += VltFile::genFileEntry(f);
    }

    vltFileNew.setDataHash(); // need for the next if
    vltFileNew.save();

    if (vltFileNew.data.hashFile != vltFileNow.data.hashFile)
    {
        cout << "update: ";
        if (files_chgd) ++updated;
    }
    else
        cout << "modify: ";

    cout << dir->fullName.string() << " [U/D/F]="
         << updated << '/' << dirs_chgd.size()
         << '/' << files_chgd << '\n';

    return updated;
}

void vault_update()
{
    jadd::Files tFiles;
    jadd::DirNode dirTree;
    tFiles.dirTree = &dirTree;
    jadd::loadFrom(".", tFiles);
    cout << "read " << tFiles.files.size() << " files\n";

    vault_updateR_cntr = 0;
    jadd::DirNode * dir = tFiles.dirTree;
    int ret = vault_updateR(tFiles, dir);

    cout << "updated " << ret << " entries";
}

