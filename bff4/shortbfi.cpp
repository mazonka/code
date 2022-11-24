#include <iostream>
#include <string>
#include <map>

int main() {

     std::string c;
     std::map<int,char> m;

     std::getline(std::cin,c,'!');

     int p = 0, l;

     for( int i = 0; i < c.size(); i++ ){
       l=1;
       if( c[i] == ']' && m[p] ) while(l){ i--;l+=(c[i]==']')-(c[i]=='['); }
       if( c[i] == '[' && !m[p]) while(l){ i++;l-=(c[i]==']')-(c[i]=='['); }
       if( c[i] == '+' ) m[p]++;
       if( c[i] == '-' ) m[p]--;
       if( c[i] == '.' ) std::cout << m[p];
       if( c[i] == ',' ) std::cin.get(m[p]);
       if( c[i] == '>' ) p++;
       if( c[i] == '<' ) p--;
     }
}
