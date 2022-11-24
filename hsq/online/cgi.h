#ifndef QUERY_H__
#define QUERY_H__

#include <map>
#include <string>
using std::string;


typedef std::map<string,string> mss;

class cgi
{
	mss get, post;
public:
	string IP, REMOTE_ADDR, HTTP_X_FORWARDED_FOR;

	cgi();
	string ip() const { return IP; }
	string operator[](const string &s) const;

	static void cl(int ac, char *av[]);
	string dump() const;

	const mss & mss_get() const { return get; }
	const mss & mss_post() const { return post; }
};

bool mainhtml0(void (*f)());
string file2str(string fname);

#endif
