 /* Stack based BF interpreter (DBFI version) */
 /* Oleg Mazonka 2009 */

 int getchar();
 int putchar(int);
 
 struct cell
 {
    struct cell *p, *n;
    int v;
 };
 typedef struct cell cell;
 
 cell *ip, *mp;
 
 void run()
 {
    while(ip)
    {
       int lev=1;
       if( ip->v == '+' ) ++mp->v;
       else if( ip->v == '-' ) --mp->v;
       else if( ip->v == '.' ) putchar(mp->v);
       else if( ip->v == ',' ) mp->v = getchar();
       else if( ip->v == '<' ) mp=mp->p;
       else if( ip->v == '>' )
       {
          if( !mp->n )
          {
             cell m; m.p=mp; 
             m.n=0; m.v=0;
             mp->n=&m; mp=&m;
             ip=ip->n;
             run();
             return;
          }
          mp=mp->n;
       }
       else if( ip->v == '[' && !mp->v )
          while(lev){ ip=ip->n; lev-=(ip->v==']')-(ip->v=='['); }
       else if( ip->v == ']' && mp->v )
          while(lev){ ip=ip->p; lev+=(ip->v==']')-(ip->v=='['); }
 
       ip=ip->n;
    }
 }
 
 void readp(cell *p)
 {
    int cmd = getchar();
    if( cmd == '!' || cmd<0 )
    {
       cell m; m.p=m.n=0;
       m.v=0; mp=&m;
       run();
    }else
    {
       cell c; c.p = p;
       c.n=0; c.v=cmd;
       if(p) p->n = &c;
       else ip = &c;
       readp(&c);
    }
 }
 
 int main(){ readp(0); }
 
