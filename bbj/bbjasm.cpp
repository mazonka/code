// BitBitJump macro assembler
// Oleg Mazonka 2009

#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <cctype>
#include <map>
#include <set>
#include <algorithm>

using std::string;
using std::vector;
using std::map;
using std::set;
using std::find;
using std::cout;
using std::istream;
using std::ostream;
using std::pair;

namespace g{
    int base=32, base_k, base_w;
    int line=0;
    string name;
    int format=0;
    bool conout = false;
}

void usage(int=1);
void setbase();
string generate_output_name(string in);

string tos(int x){ std::ostringstream os; os<<x; return os.str(); }
string tob(int x);

struct Word;
struct Value{
    enum type { STR, QST, NUM };
    type typ;
    int ival;
    string sval;
    Value() {}
    Value(type t, const string &s): typ(t), sval(s) {}
    Value(type t, int i): typ(t), ival(i) {}
    string outt() const;
};

struct Bit
{
    enum type { INT, STR };
    type typ;
    int ival;
    string sval;
    Bit(): typ(INT), ival(0) {}
    explicit Bit(int x): typ(INT), ival(x) {}
    explicit Bit(const string & s): typ(STR), sval(s) {}
    string outt() const;
};

struct Word
{
    int addr;
    set<string> labels;
    Value value;
    Bit bit;
    Word * word;
    Word(const Value &v): value(v), bit(), word(0) {}
    string outt() const;
    string outv(string (*)(int)) const;
    string getMainLabel() const;
};

string Word::getMainLabel() const
{
    return labels.size()==1? *labels.begin():string();
}

class Directive
{
    int line;
    string file;
public:
    Directive(): line(g::line), file(g::name) {}
    virtual ~Directive() {}
    string er() const;
};
string Directive::er() const { return "("+file+":"+tos(line)+")"; }

struct Instruction : Directive
{
    Word *a,*b,*c;
    Instruction() : a(0), b(0), c(0) {}
    Instruction(Word *x, Word *y, Word *z) : a(x), b(y), c(z) {}
    string outt() const;
    string outv(string (*)(int)) const;

    string getMainLabel() const { return a->getMainLabel(); }

private:
    Instruction(const Instruction&);
};


struct MacroCmd : Directive
{
    set<string> labels;
    string name;
    vector<Value> args;
    MacroCmd(const set<string> &labs, const string &n, const vector<Value> &a) 
        : labels(labs), name(n), args(a) {}

    string getMainLabel() const ;
};

string MacroCmd::getMainLabel() const 
{ return labels.size()==1? *labels.begin():string(); }

struct MacroDef : Directive
{
    string name;
    vector<string> args, exts;
    vector<Directive*> dirs;
    MacroDef(const string &n, const vector<string> &a, 
        const vector<string> &e, const vector<Directive*> &d)
        : name(n), args(a), exts(e), dirs(d) {}

    vector<Directive*> instantiate(MacroCmd * m);
};

string Value::outt() const
{
    if( typ==STR ) return sval;
    if( typ==QST )
    {	
        if( ival != 1 )
            return tos(ival)+"?";
        return "?";
    }
    if( typ==NUM ) return tos(ival);
    throw string()+"Value::out: error";
}

string Word::outt() const
{
    string r;
    for( set<string>::const_iterator i = labels.begin(); i!=labels.end(); i++ )
        r += (*i)+":";

    if( value.typ == Value::STR )
    {
        if( word )
            r += tos(word->addr);
        else
            throw "Undefined word value '"+value.sval+"'";
    }
    else
        r += value.outt();

    r += bit.outt();

    return r;
}

string Bit::outt() const 
{
    if( typ != INT )
        throw string()+"Internal error: Bit::outt";

    if( ival ) return "'"+tos(ival);
    return "";
}

string Word::outv(string (*f)(int)) const 
{
    if( bit.typ != Bit::INT )
        throw "Bit name '"+bit.sval+"' unresolved";

    int b = bit.ival;

    if( value.typ==Value::NUM )
        return (*f)(value.ival+b);

    if( !word )
    {
        if( value.typ==Value::STR )
            throw string()+"unresolved '"+value.sval+"'";
    }

    if( value.typ==Value::STR ) return (*f)(word->addr+b);

    if( value.typ==Value::QST ) return (*f)(word->addr+ value.ival*g::base+b);

    throw string()+"Word::outv: error";
}


string Instruction::outt() const { return a->outt()+' '+b->outt()+' '+c->outt(); }
string Instruction::outv(string (*f)(int)) const { return a->outv(f)+' '+b->outv(f)+' '+c->outv(f); }

vector<Directive*> readcc(istream &is, map<string,Directive*> * conds);
void writecc(ostream &os, vector<Directive*> & instructions);
map<string,Word*> buildLabTable(const vector<Directive*> & instructions, bool erase);
void assignAddress(vector<Directive*> & instructions);
string resolveReferences(vector<Directive*> & ins, map<string,Word*> & labs);
void resolveMacroCmd(vector<Directive*> & instructions);
void checkMacroRedef(vector<Directive*> & ins);

int main(int ac, char *av[])
{
    try{
        if( ac < 2 ) usage(0);

        int input_i = 1;

        while(1)
        {
            string s = string(av[input_i]);
            if( s.empty() || s[0]!='-' ) break;
            else
                if( s=="-b" ) g::format=1;
                else if( s=="-t" ) g::format=2;
                else if( s=="-c" ) g::conout=true;
                else if( s=="-8" ) g::base=8;
                else if( s=="-16" ) g::base=16;
                else if( s=="-32" ) g::base=32;
                else usage();

                if( ++input_i >= ac ) usage();
        }

        setbase();

        string input_name = av[input_i];

        string output_name;
        if( input_i+1 < ac )
            output_name = av[input_i+1];
        else
            output_name = generate_output_name(input_name);

        std::ifstream in(input_name.c_str());

        if( !in )
        {
            cout<<"cannot open "<<input_name<<'\n';
            return 1;
        }

        g::name = input_name;

        map<string,Directive*> conds;
        vector<Directive*> vi = readcc(in,&conds);

        g::line = -1;

tryagain:
        checkMacroRedef(vi);

        resolveMacroCmd(vi);

        map<string,Word*> labtable = buildLabTable(vi,false);
        string unresed = resolveReferences(vi,labtable);

        if( unresed != "" )
        {
            if( conds.find(unresed)!=conds.end() )
            {
                if( !g::conout ) std::cout<<"Resolved conditional '"<<unresed<<"'\n";
                vi.push_back( conds[unresed] );
                goto tryagain;
            }
            std::cerr<<"Error: Unresolved '"<<unresed<<"'\n";
        }

        assignAddress(vi);

        ostream * pout = &std::cout;
        std::ofstream fout;

        if( !g::conout )
        {
            fout.open(output_name.c_str());

            if( !fout )
                throw "cannot open "+output_name;
            pout = &fout;
        }

        std::ostringstream os;
        writecc(os,vi);
        (*pout)<<os.str();

    }
    catch(string s)
    {
        if( g::line < 0 )
            std::cerr<<"Error: "<<s<<'\n';
        else
            std::cerr<<"Error: ("<<g::name<<":"<<g::line<<") "<<s<<'\n';
        return 3;
    }
    catch(...)
    {
        std::cerr<<"Unknown error\n";
        return 4;
    }
}

void usage(int r)
{
    cout<<"Useage: bbjasm [-{8|16|32}] [-b] [-t] [-c] input.bbj [output.run]\n";
    cout<<"\t-<n> base 8, 16, or 32 bits; default -32\n";
    cout<<"\t-b output binary format\n";
    cout<<"\t-t output text format\n";
    cout<<"\t-c output to stdout\n";
    exit(r);
}

void setbase()
{
    g::base_w = g::base-1;
    if( g::base == 8 )	g::base_k = 3;
    else if( g::base == 16 ) g::base_k = 4;
    else if( g::base == 32 ) g::base_k = 5;
    else 
        usage();
}

string generate_output_name(string in)
{
    const string ext = ".run";
    string::size_type i = in.rfind(".");

    if( i == string::npos )
        return in+ext;

    return in.substr(0,i)+ext;
}


pair<bool,Instruction*> getinstr(const string &s);
bool isEmpty(const string &s);
pair<bool,MacroDef*> getmacrodef(const string &s, istream &is);
pair<bool,MacroCmd*> getmacrocmd(const string &s);
pair<bool,vector<Directive*> > getinclude(const string &s, map<string,Directive*> * conds);
bool consumecondition(string &s);

vector<Directive*> readcc(istream &is, map<string,Directive*> * conds)
{
    vector<Directive*> ret;
    while( true )
    {
        string s;
        getline(is,s);
        if( !is ) break;
        g::line++;

        // replace ?? macro
        string::size_type i= s.find("??");
        if( i != string::npos ) s.replace(i,2,tos(g::base));

        i= s.find("#");
        if( i != string::npos )	s = s.substr(0,i);

        bool conditional = consumecondition(s);

        pair<bool,Instruction*> inst = getinstr(s);
        if( inst.first )
        {
            if( !conditional )
                ret.push_back(inst.second); 
            else if( conds )
                (*conds)[inst.second->getMainLabel()] = inst.second;
            continue;
        }
        if( isEmpty(s) )
            continue;

        pair<bool,vector<Directive*> > incl = getinclude(s,conds);
        if( incl.first )
        {
            ret.insert( ret.end(), incl.second.begin(), incl.second.end() );
            continue;
        }

        pair<bool,MacroDef*> mdef = getmacrodef(s,is);
        if( mdef.first )
        {
            ret.push_back(mdef.second);
            continue;
        }

        pair<bool,MacroCmd*> mcmd = getmacrocmd(s);
        if( mcmd.first )
        {
            if( mcmd.second->name == "end" )
            {
                delete mcmd.second;
                break;
            }

            if( !conditional )
                ret.push_back(mcmd.second); 
            else if( conds )
                (*conds)[mcmd.second->getMainLabel()] = mcmd.second;
            continue;
        }

        throw string()+"Unknown instruction";
    }
    return ret;
}


void eatspace(const string &s, int &i)
{ while( i<(int)s.size() && std::isspace(s[i]) ) i++; }

bool consumecondition(string &s)
{
    int i=0;
    eatspace(s,i);
    if( i>=(int)s.size() || s[i]!=':' ) return false;
    s = s.substr(i+1);
    return true;
}

pair<bool,Word*> getword(const string &s, int &i);
pair<bool,Instruction*> getinstr(const string &s)
{
    pair<bool,Instruction*> ret;

    int i=0;
    pair<bool,Word*> a = getword(s,i);

    if( !a.first ) return pair<bool,Instruction*>(false,(Instruction*)0);

    pair<bool,Word*> b = getword(s,i);

    if( !b.first )
        throw string()+"instruction must have at least 2 operands";

    pair<bool,Word*> c = getword(s,i);

    if( !c.first )
    {
        c.second = new Word(Value(Value::QST,1));
    }

    ret.first = true;

    ret.second = new Instruction(a.second,b.second,c.second);

    return ret;
}

pair<bool,string> getlabel(const string &s, int &i);
set<string> getlabels(const string &s, int &i);
pair<bool,Value> getvalue(const string &s, int &i);
pair<bool,Bit> getbit(const string &s, int &i);
void cutspace(string &s)
{ while( s.size() && std::isspace(s[s.size()-1]) ) s.erase(s.size()-1); }

pair<bool,Word*> getword(const string &s, int &i)
{
    int j=i;

    eatspace(s,j);

    set<string> labs = getlabels(s,j);

    eatspace(s,j);

    pair<bool,Value> val = getvalue(s,j);

    if( !val.first )
        return pair<bool,Word*>(false,(Word*)0);

    pair<bool,Bit> bit = getbit(s,j);

    i=j;
    Word * w = new Word(val.second);
    w->labels = labs;

    w->bit = Bit();
    if( bit.first )
        w->bit = bit.second;

    return pair<bool,Word*>(true,w);
}

pair<bool,int> getnumber(const string &s, int &i);
pair<bool,string> getstring(const string &s, int &i);
pair<bool,Bit> getbit(const string &s, int &i)
{
    int j = i;
    if( j<(int)s.size() && s[j]=='\'' )
    {
        pair<bool,int> num = getnumber(s,++j);

        if( num.first )
        {
            i = j;
            return pair<bool,Bit>(true,Bit(num.second));
        }

        pair<bool,string> str = getstring(s,j);

        if( str.first )
        {
            i = j;
            return pair<bool,Bit>(true,Bit(str.second));
        }

        throw string()+"expecting number or arg name after [']";
    }

    return pair<bool,int>(false,0);
}


pair<bool,int> getint(const string &s, int &i);
pair<bool,int> getquest(const string &s, int &i);
pair<bool,Value> getvalue(const string &s, int &i)
{
    pair<bool,string> name = getstring(s,i);

    if( name.first )
        return pair<bool,Value>(true,Value(Value::STR,name.second));

    pair<bool,int> quest = getquest(s,i);

    if( quest.first )
        return pair<bool,Value>(true,Value(Value::QST,quest.second));

    pair<bool,int> num = getint(s,i);

    if( num.first )
        return pair<bool,Value>(true,Value(Value::NUM,num.second));

    return pair<bool,Value>(false,Value());
}


bool getthischar(const string &s, int &i, char c);
pair<bool,int> getint(const string &s, int &i)
{
    int j=i;

    bool minus = getthischar(s,j,'-');
    pair<bool,int> integer = getnumber(s,j);

    if( !integer.first )
        return pair<bool,int>(false,0);

    i=j;
    if(minus)
        return pair<bool,int>(true,-integer.second);

    return pair<bool,int>(true,integer.second);
}

bool getthischar(const string &s, int &i, char c)
{
    if( i < (int)s.size() && s[i]==c )
    {
        i++;
        return true;
    }

    return false;
}


pair<bool,string> getlabel(const string &s, int &i)
{
    int j = i;
    pair<bool,string> name = getstring(s,j);

    if( !name.first )
        return pair<bool,string>(false,"");	

    bool col = getthischar(s,j,':');

    if( !col )
        return pair<bool,string>(false,"");

    i=j;
    return pair<bool,string>(true,name.second);
}

set<string> getlabels(const string &s, int &i)
{
    set<string> ret;
    eatspace(s,i);
    while(1)
    {
        pair<bool,string> lab = getlabel(s,i);
        if( !lab.first ) break;
        ret.insert( lab.second );
        eatspace(s,i);
    }
    return ret;
}

pair<bool,int> getnumber(const string &s, int &i)
{
    string r;
    while( i<(int)s.size() && std::isdigit(s[i]) )
    {
        r += s[i++];
    }

    if( r=="" )
        return pair<bool,int>(false,0);

    return pair<bool,int>( true, atoi(r.c_str()) );
}

pair<bool,int> getquest(const string &s, int &i)
{
    int j=i;
    pair<bool,int> n = getint(s,j);

    if( !n.first )
    {
        bool q = getthischar(s,j,'?');
        if( !q )
            return pair<bool,int>(false,0);
        i=j;
        return pair<bool,int>(true,1);
    }

    bool q = getthischar(s,j,'?');

    if( !q )
        return pair<bool,int>(false,0);

    i=j;
    return pair<bool,int>(true,n.second);
}

pair<bool,string> getalnum(const string &s, int &i);
pair<bool,string> getstring(const string &s, int &i)
{
    if( !(i<(int)s.size()) || !(std::isalpha(s[i]) || s[i]=='_') )
        return pair<bool,string>(false,"");

    return getalnum(s,i);
}

pair<bool,string> getalnum(const string &s, int &i)
{
    string r;
    while( i<(int)s.size() && ( std::isalnum(s[i]) || s[i]=='_' ) )
        r += s[i++];

    return pair<bool,string>(true,r);
}

void writecc(ostream &os, vector<Directive*> & ins)
{
    for( int i=0; i<(int)ins.size(); i++ )
    {

        if ( dynamic_cast<MacroDef*>(ins[i]) )
            continue;

        if ( dynamic_cast<MacroCmd*>(ins[i]) )
            throw string()+"Internal error: unresolved macro";

        Instruction* in = dynamic_cast<Instruction*>(ins[i]);
        if( !in )
            throw string()+"Internal error: undefined directive";

        try{
            if( g::format==0 ) 
                os<<in->outv(tos)<<'\n';
            else if( g::format==1 )
                os<<in->outv(tob)<<'\n';
            else if( g::format==2 )
                os<<in->outt()<<'\n';

            else
                throw string()+"format undefined";
        } catch(string s) {
            throw in->er()+' '+s;
        }	
    }
}


void buildLabTable(map<string,Word*> &m, Word *w, bool erase);
map<string,Word*> buildLabTable(const vector<Directive*> & ins, bool erase)
{
    map<string,Word*> ret;
    for( int i=0; i<(int)ins.size(); i++ )
    {
        if ( dynamic_cast<MacroDef*>(ins[i]) )
            continue;

        if ( dynamic_cast<MacroCmd*>(ins[i]) )
            continue;

        Instruction* in = dynamic_cast<Instruction*>(ins[i]);
        if( !in )
            throw string()+"Internal error: undefined directive";

        try{
            buildLabTable(ret,in->a,erase);
            buildLabTable(ret,in->b,erase);
            buildLabTable(ret,in->c,erase);
        }catch( string s ){
            throw in->er()+' '+s;
        }
    }
    return ret;
}

void buildLabTable(map<string,Word*> &m, Word *w, bool erase)
{
    set<string> todel;
    for( set<string>::const_iterator i=w->labels.begin(); i!=w->labels.end(); i++ )
    {
        if( m.find(*i) != m.end() )
            throw "label '"+(*i)+"' already defined";
        m[*i] = w;
        todel.insert(*i);
    }

    if( !erase ) return;

    for( set<string>::const_iterator i=todel.begin(); i!=todel.end(); i++ )
        w->labels.erase(*i);
}


void assignAddress(vector<Directive*> & ins)
{
    unsigned long addr=0;
    for( int i=0; i<(int)ins.size(); i++ )
    {

        if ( dynamic_cast<MacroDef*>(ins[i]) )
            continue;

        if ( dynamic_cast<MacroCmd*>(ins[i]) )
            throw string()+"Internal error: unresolved macro";

        Instruction* in = dynamic_cast<Instruction*>(ins[i]);
        if( !in )
            throw string()+"Internal error: undefined directive";

        in->a->addr = addr; addr+=g::base;
        in->b->addr = addr; addr+=g::base;
        in->c->addr = addr; addr+=g::base;
    }

    unsigned long max = (1UL << g::base_w);

    if( addr >= max )
        throw string()+"word base "+tos(g::base)+" is not enough to hold "
        "max address "+ tos(addr) +" >= "+tos(max);
}

string resolveReference(Word *w, map<string,Word*> & labs);
string resolveReferences(vector<Directive*> & ins, map<string,Word*> & labs)
{

    string r;

    for( int i=0; i<(int)ins.size(); i++ )
    {

        if ( dynamic_cast<MacroDef*>(ins[i]) )
            continue;

        if ( dynamic_cast<MacroCmd*>(ins[i]) )
            throw string()+"Internal error: unresolved macro";

        Instruction* in = dynamic_cast<Instruction*>(ins[i]);
        if( !in )
            throw string()+"Internal error: undefined directive";

        string ra = resolveReference(in->a,labs);
        string rb = resolveReference(in->b,labs);
        string rc = resolveReference(in->c,labs);

        if( ra != "" ) r = ra;
        if( rb != "" ) r = rb;
        if( rc != "" ) r = rc;
    }

    return r;
}

string resolveReference(Word *w, map<string,Word*> & labs)
{
    string r;
    if( w->value.typ == Value::STR )
    {
        if( labs.find( w->value.sval )== labs.end() ) 
            r = w->value.sval;
	else
	    w->word = labs[w->value.sval];
    }

    if( w->value.typ == Value::QST )
        w->word = w;

    return r;
}

string tob(int x)
{
    if( x<-1 )
        throw string()+"negative addresses not supported except '-1'";

    string s;
    for( int i=0; i<g::base; i++ )
    {
        int j=g::base-i-1;
        if( x & (1<<j) )
            s += "1";
        else
            s += "0";
    }
    return s;
}

bool isEmpty(const string &s)
{
    string::size_type i=0;
    while( i<s.size() && std::isspace(s[i]) ) i++;

    return i>=s.size();
}

vector<string> readargsstr(const string &s, int &i)
{
    vector<string> args;
    while(true)
    {
        eatspace(s,i);
        pair<bool,string> arg = getstring(s,i);
        if( !arg.first ) break;
        args.push_back(arg.second);
    }
    return args;
}

vector<Value> readargsval(const string &s, int &i)
{
    vector<Value> args;
    while(true)
    {
        eatspace(s,i);
        pair<bool,Value> arg = getvalue(s,i);
        if( !arg.first ) break;
        args.push_back(arg.second);
    }
    return args;
}
pair<bool,MacroDef*> getmacrodef(const string &s, istream &is)
{
    pair<bool,MacroDef*> ret(false,(MacroDef*)0);

    int i=0;
    eatspace(s,i);
    if( i>=(int)s.size() || s[i]!='.' ) return ret;
    i++;

    pair<bool,string> str = getstring(s,i);

    if( !str.first ) return ret;

    if( str.second != "def" ) return ret;

    eatspace(s,i);

    pair<bool,string> mname = getstring(s,i);

    if( !mname.first )
        throw string()+"Macro defintion expecting name";

    vector<string> args, exts;
    args = readargsstr(s,i);

    if( i<(int)s.size() && s[i]==':' )
    {
        i++;
        exts = readargsstr(s,i);
    }

    vector<Directive*> dirs = readcc(is,0);

    return pair<bool,MacroDef*>(true,new MacroDef(mname.second,args,exts,dirs) );
}

pair<bool,MacroCmd*> getmacrocmd(const string &s)
{
    pair<bool,MacroCmd*> ret(false,(MacroCmd*)0);

    int i=0;

    eatspace(s,i);

    set<string> labs = getlabels(s,i);

    eatspace(s,i);

    if( i>=(int)s.size() || s[i]!='.' ) return ret;
    i++;

    pair<bool,string> name = getstring(s,i);

    if( !name.first )
        throw string()+"Macro directive expecting name";

    vector<Value> args;
    args = readargsval(s,i);

    return pair<bool,MacroCmd*>(true,new MacroCmd(labs,name.second,args) );
}


void resolveMacroCmd(vector<Directive*> & ins, int icmd, MacroCmd * cmd, MacroDef *def);
void resolveMacroCmd(vector<Directive*> & ins)
{
start:

    for( int i=0; i<(int)ins.size(); i++ )
    {
        if ( dynamic_cast<MacroDef*>(ins[i]) )
            continue;

        if ( dynamic_cast<Instruction*>(ins[i]) )
            continue;

        MacroCmd* m = dynamic_cast<MacroCmd*>(ins[i]);
        if( !m )
            throw string()+"Internal error: undefined directive";

        // find def
        for( int j=0; j<(int)ins.size(); j++ )
        {
            MacroDef* def = dynamic_cast<MacroDef*>(ins[j]);
            if( !def )
                continue;

            if( m->name != def->name )
                continue;

            resolveMacroCmd(ins,i,m,def);
            goto start;
        }

        throw m->er()+" Cannot find macro def for '"+m->name+"'";
    }
}

void resolveMacroCmd(vector<Directive*> & ins, int icmd, MacroCmd * cmd, MacroDef *def)
{

    vector<Directive*> dirs = def->instantiate(cmd);

    if( dirs.empty() && cmd->labels.size() )
        throw cmd->er()+" cannot label empty macro '"+cmd->name+"'";

    else if( cmd->labels.size() )
    {
        Instruction * in = dynamic_cast<Instruction*>(dirs[0]);
        MacroCmd * mac = dynamic_cast<MacroCmd*>(dirs[0]);
        if( in )
            in->a->labels.insert( cmd->labels.begin(), cmd->labels.end() );
        else if( mac )
            mac->labels.insert( cmd->labels.begin(), cmd->labels.end() );
        else
            throw cmd->er()+" cannot label macro starting with def '"+cmd->name+"'";
    }

    ins.erase( ins.begin()+icmd );
    ins.insert( ins.begin()+icmd, dirs.begin(), dirs.end() );
}

vector<Directive*> copynodef(const vector<Directive*>& dirs);

class Resolver
{
    const map<string,Word*> & labws;
    const map<string,MacroCmd*> & labms;
    const map<string,string> & unames;
    const vector<string> &args;
    const vector<Value> &fargs;
    const vector<string> &exts;
public:
    Resolver(const map<string,Word*> & l, 
        const map<string,MacroCmd*> & c,
        const map<string,string> & u,
        const vector<string> &a, const vector<Value> &f,
        const vector<string> &e) :
    labws(l), labms(c), unames(u), args(a), fargs(f), exts(e) {}

    void resolve(vector<Directive*> &dirs);
    void resolve(Instruction * ins);
    void resolve(MacroCmd * mac);
    void resolve(Word * w);
    void resolve(Value * s);
    void resolve(Bit * b);
};

map<string,string> buildUniqueNames(const map<string,Word*> &m1, const map<string,MacroCmd*> &m2);

map<string,MacroCmd*> buildLabTableMac(const vector<Directive*> & ins, bool erase);
vector<Directive*> MacroDef::instantiate(MacroCmd * maccmd)
{
    const vector<Value> &formal = maccmd->args;

    if( formal.size() != args.size() )
        throw maccmd->er()+" macro instantiation argument mismatch '"+name+"'";

    vector<Directive*> ret;

    ret = copynodef(dirs);

    map<string,Word*> labws = buildLabTable(ret,true);
    map<string,MacroCmd*> labms = buildLabTableMac(ret,true);

    map<string,string> unames = buildUniqueNames(labws,labms);

    Resolver resolver(labws,labms,unames,args,formal,exts);

    try{

        resolver.resolve(ret);

    }catch(string s)
    {
        throw s+" when "+er()+" instantiates "+maccmd->er();
    }

    return ret;
}

Directive* copyof(Directive* d);
vector<Directive*> copynodef(const vector<Directive*>& ins)
{
    vector<Directive*> ret;

    for( int i=0; i<(int)ins.size(); i++ )
    {
        if ( dynamic_cast<MacroDef*>(ins[i]) )
            continue;

        ret.push_back( copyof(ins[i]) );
    }

    return ret;
}

Directive* copyof(Directive* d)
{
    Instruction * inst = dynamic_cast<Instruction*>(d);

    if( inst )
        return new Instruction(
        new Word(*inst->a),
        new Word(*inst->b),
        new Word(*inst->c)
        );

    MacroCmd * mac = dynamic_cast<MacroCmd*>(d);

    if( mac )
        return new MacroCmd(*mac);

    throw string()+"copyof internal error";
}

string getuniquelab();
map<string,string> buildUniqueNames
(const map<string,Word*> &m1, const map<string,MacroCmd*> &m2)
{
    map<string,string> r;

    for( map<string,Word*>::const_iterator i = m1.begin(); i!=m1.end(); ++i )
        r[ i->first ] = getuniquelab();

    for( map<string,MacroCmd*>::const_iterator i = m2.begin(); i!=m2.end(); ++i )
        r[ i->first ] = getuniquelab();

    return r;
}

void Resolver::resolve(vector<Directive*> & dirs)
{
    for( int i=0; i<(int)dirs.size(); i++ )
    {

        MacroCmd* m = dynamic_cast<MacroCmd*>(dirs[i]);
        if( m )
        {
            resolve(m);
            continue;
        }

        Instruction *in = dynamic_cast<Instruction*>(dirs[i]);
        if( in )
        {
            resolve(in);
            continue;
        }

        throw string()+"internal error "+ tos(__LINE__);
    }
}


void Resolver::resolve(Instruction* ins)
{
    try{
        resolve(ins->a);
        resolve(ins->b);
        resolve(ins->c);
    } catch ( string s ){
        throw ins->er()+' '+s;
    }
}

void Resolver::resolve(Word* w)
{
    if( w->value.typ == Value::STR )
        resolve(&w->value);

    if( w->bit.typ == Bit::STR )
        resolve(&w->bit);
}

void Resolver::resolve(Value * s)
{
    {
        map<string,Word*>::const_iterator it = labws.find(s->sval);
        if( it != labws.end() )
        {
            string newlab = unames.find(s->sval)->second;
            it->second->labels.erase(s->sval);
            s->sval = newlab;
            it->second->labels.insert(newlab);
            return;
        }
    }

    {
        map<string,MacroCmd*>::const_iterator it = labms.find(s->sval);
        if( it != labms.end() )
        {
            string newlab = unames.find(s->sval)->second;
            it->second->labels.erase(s->sval);
            s->sval = newlab;
            it->second->labels.insert(newlab);
            return;
        }
    }

    vector<string>::const_iterator its = find(args.begin(),args.end(),s->sval);

    if( its != args.end() )
    {
        vector<string>::size_type offset = its - args.begin();
        *s = fargs[offset];
        return;
    }

    its = find(exts.begin(),exts.end(),s->sval);
    if( its != exts.end() ) return;

    throw "undefined label '"+s->sval+"' in macro instance";
}

void Resolver::resolve(Bit * b)
{
    vector<string>::const_iterator it = find(args.begin(),args.end(),b->sval);

    if( it == args.end() )
        throw "Bit '"+b->sval+"' cannot be resolved within arguments";

    vector<string>::size_type offset = it - args.begin();

    Value v = fargs[offset];

    if( v.typ == Value::NUM )
    {
        *b = Bit(v.ival);
        return;
    }

    throw "Bit '"+b->sval+"' resolves into no number";
}

void Resolver::resolve(MacroCmd *mac)
{
    for( int i=0; i<(int)mac->args.size(); i++ )
    {
        if( mac->args[i].typ != Value::STR ) continue;
        resolve( &mac->args[i] );
    }
}

string getuniquelab()
{
    static int x = 0;
    return "_"+tos(++x);
}

pair<bool,vector<Directive*> > getinclude(const string &s, map<string,Directive*> * conds)
{
    pair<bool,vector<Directive*> > ret(false,vector<Directive*>());

    int i=0;
    eatspace(s,i);
    if( i>=(int)s.size() || s[i]!='.' ) return ret;
    i++;

    pair<bool,string> str = getstring(s,i);

    if( !str.first ) return ret;

    if( str.second != "include" ) return ret;

    eatspace(s,i);

    string fname = s.substr(i);
    cutspace(fname);

    int line0 = g::line;
    string name0 = g::name;
    g::name = fname;
    g::line=0;

    std::ifstream in(fname.c_str());
    if( !in )
        throw "Cannot open '"+fname+"'";

    ret.first = true;
    ret.second = readcc(in,conds);
    g::line = line0;
    g::name = name0;
    return ret;
}


void buildLabTableMac(map<string,MacroCmd*> &m, MacroCmd *w, bool erase);
map<string,MacroCmd*> buildLabTableMac(const vector<Directive*> & ins, bool erase)
{
    map<string,MacroCmd*> ret;
    for( int i=0; i<(int)ins.size(); i++ )
    {
        if ( dynamic_cast<MacroDef*>(ins[i]) )
            continue;

        if ( dynamic_cast<Instruction*>(ins[i]) )
            continue;

        MacroCmd* in = dynamic_cast<MacroCmd*>(ins[i]);
        if( !in )
            throw string()+"Internal error: undefined directive";

        buildLabTableMac(ret,in,erase);
    }
    return ret;
}

void buildLabTableMac(map<string,MacroCmd*> &m, MacroCmd *w, bool erase)
{
    set<string> todel;
    for( set<string>::const_iterator i=w->labels.begin(); i!=w->labels.end(); i++ )
    {
        if( m.find(*i) != m.end() )
            throw w->er()+" label '"+(*i)+"' already defined";
        m[*i] = w;
        todel.insert(*i);
    }

    if( !erase ) return;

    for( set<string>::const_iterator i=todel.begin(); i!=todel.end(); i++ )
        w->labels.erase(*i);
}

void checkMacroRedef(vector<Directive*> & ins)
{
    for( int i=0; i<(int)ins.size(); i++ )
    {
        MacroDef* def1 = dynamic_cast<MacroDef*>(ins[i]);
        if( !def1 ) continue;

        for( int j=0; j<(int)ins.size(); j++ )
        {
            if( i==j ) continue;
            MacroDef* def2 = dynamic_cast<MacroDef*>(ins[j]);
            if( !def2 ) continue;

            if( def2->name == def1->name )
                throw "Macro "+def1->name+" defined more than once "+def1->er()+def2->er();
        }
    }
}

