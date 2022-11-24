#include <time.h>
#include <stdlib.h>
#include <stdio.h>

const int DB=0;
const int FRONT_MAYBE_ZERO=1;

typedef struct
{
	char x[5];
} csnum;

typedef struct
{
	int x[6000];
	int sz;
} vect;

vect gn;
int hint;

int valid(int x);
int guess();
void dump();

void getbc(int n1, int n2, int *b, int *c);
vect process(const vect * pn, int g, int b, int c);
void init(vect *n);



csnum mks[10000];
void initmks();

int main(int ac, char *av[])
{
	char a;
	char *t[10]={ "first", "second", "third", "fourth", "fifth", 
		"sixth", "seventh", "eighth", "ninth", "tenth" };

	int ctr=0;
	int rnd;
	int g;


	if( ac>1 ) hint = atoi(av[1]);

	initmks();
	init(&gn);

	printf("Usage: bullcow [help_number]\n\n");

	while(1)
	{
		printf("Have you thought a number? (type 'y' and enter) ");
		a = '\0';
		scanf("%c",&a);
		if( a=='y' || a=='Y' ) break;
	}

	ctr=0;
	while(1)
	{
		int b=0,c=0;

		if( gn.sz > 700 )
			g = gn.x[ clock()%gn.sz ];
		else
			g = guess();

		printf("My ");
		if( ++ctr<11 ) printf("%s",t[ctr-1]); else printf("%dth",ctr);
		printf(" guess is %s",mks[g].x);

		if( hint )
		{
			getbc(g,hint,&b,&c);
			printf("  [ for %s bulls=%d cows=%d ]",mks[hint].x,b,c);
		}

		printf("\n");

		printf("How many guessed in place (bulls): ");
		scanf("%d",&b);
		printf("How many guessed not in place (cows): ");
		scanf("%d",&c);

		gn = process(&gn,g,b,c);

		if( gn.sz==0 )
		{
			printf("Impossible - you made an error\n");
			break;
		}

		if( gn.sz==1 )
		{
			printf("Your number is %s",mks[gn.x[0]].x);
			break;
		}

	}
}

void init(vect *n)
{
	int i;
	n->sz=0;
	for( i=0; i<10000; i++ )
	{
		if( valid(i) ) n->x[n->sz++] = i;
	}
}

int valid(int x)
{
	const char *s = mks[x].x;

	int i,j;

	if( !FRONT_MAYBE_ZERO && x<1000 ) return 0;

	for( i=0; i<4; i++ )
	for( j=0; j<4; j++ )
	{
		if( i==j ) continue;
		if( s[i]==s[j] ) return 0;
	}

	return 1;
}

void dump()
{
	int i;
	printf("[");
	for( i=0; i<gn.sz; i++ )
	printf(" %s",mks[gn.x[i]].x);
	printf(" ]\n");
}

void getbc(int n1, int n2, int *b, int *c)
{
	int i,j;
	const char * s1 = mks[n1].x;
	const char * s2 = mks[n2].x;

	*b=0;
	*c=0;

	for( i=0; i<4; i++ )
	for( j=0; j<4; j++ )
	{
		if( s1[i]==s2[j] )
		{
			if( i==j ) ++*b;
			else ++*c;
		}
	}
}

void getb(int n1, const char * s2, int *b)
{
	int i;
	const char * s1 = mks[n1].x;

	*b=0;

	for( i=0; i<4; i++ )
	{
		if( s1[i]==s2[i] ) ++*b;
	}
}

vect process(const vect * n, int g, int b, int c)
{
	int i,k;
	vect r;
	vect rm;

	const char * s2 = mks[g].x;

	r.sz = rm.sz = 0;

	for( i=0; i<n->sz; i++ )
	{
		int mb, mc;
		getb(n->x[i],s2,&mb);
		if( mb!=b ){ rm.x[rm.sz++]=i; continue; }
		
		getbc(n->x[i],g,&mb,&mc);
		if( mb!=b || mc!=c ) rm.x[rm.sz++]=i;
	}

	{
		for( k=0,i=0; i<n->sz; i++ )
		{
			if( k<rm.sz && rm.x[k]==i ) k++;
			else r.x[r.sz++] = n->x[i];
		}
	}

	return r;
}

int processq(const vect* n, int g, int b, int c)
{
	int i;
	int r=n->sz;

	const char * s2 = mks[g].x;
	for( i=0; i<n->sz; i++ )
	{
		int mb, mc;
		getb(n->x[i],s2,&mb);
		if( mb!=b ){ --r; continue; }
		
		getbc(n->x[i],g,&mb,&mc);
		if( mb!=b || mc!=c ) --r;
	}

	return r;
}

int guess()
{
	int i,b,c;
	int nmin = gn.sz, inmin=-1;

	const double timeout = 2000;
	double tm = clock()+timeout;

	for( i=0; i<gn.sz; i++ )
	{
		int iq;
		int nnmax=0;
		for( b=0; b<=4; b++ )
		for( c=0; c<=4-b; c++ )
		{
			if( c==1 && b+c==4 ) continue;
			iq = processq(&gn,gn.x[i],b,c);
			if( iq==0 ) continue;
			if( nnmax<iq ) nnmax = iq;
		}

		if( nnmax < nmin )
		{
			nmin = nnmax;
			inmin = i;
		}

		if( !DB ) if( inmin>=0 && (double)(clock()) > tm ) break;
	}

	if( DB ) printf("Guess took: %d\n",(int)((double)(clock())-tm+timeout));

	return gn.x[inmin];
}

void initmks()
{
	int i;
	for( i=0; i<10000; i++ )
	{
		csnum x;
		sprintf(x.x,"%04d",i);
		mks[i] = x;
	}
}

