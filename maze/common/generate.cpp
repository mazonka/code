// Theseus and Minotaur Game
// Oleg Mazonka 2010
// http://mazonka.com/maze/

#include <time.h>
#include <stdlib.h>

#include "generate.h"
#include "solver.h"

void initrand()
{
	static bool init = false;
	if(init) return;

	init = true;
	srand( (unsigned)time(0) );
	//srand( (unsigned)(0) );
}

double drnd()
{
	return (rand()%10001)/10000.0;
}

int irnd(int max)
{
	return rand()%max;
}

Generator::Generator(): 
	mCurrentSet(false),
	mSize(5,4), mDifficulty(-1), mTotal(1000), mCurrent(0),
	mFilling(0.3), mIsolation(0.5), mBorderExit(1), mVariator(0.5) 
{
}

Generator::Generator(const Generator& g, const Maze &m)
{
	*this = g;

	mCurrentSet = false;
	if( m.size() != mSize ) return;

	mCurrentMaze = m;
	mCurrentSet = true;
}

void Generator::generate(int n)
{
	initrand();

	double start = clock();
	while(1)
	{
		generate1();
		mCurrent++;
		if( isDone() ) break;

		double passed = clock();
		if( (passed-start)*1000/CLOCKS_PER_SEC > n ) break;
	}
}

void Generator::generate1()
{
	double r = drnd();
	if( !mCurrentSet || r>mVariator )
	{
		generateWalls();
		mCurrentSet = true;
	}

	variate();

	ShortSolver solver(&mCurrentMaze);
	solver.solveT(-1);
	solver.apply();

	int difficulty = mCurrentMaze.getNumStates();
	int st = solver.state();
	if( st!=3 && st!=1 ) difficulty=-1;

	if( mCurrent==0 || difficulty > mDifficulty )
	{
		mBestMaze = mCurrentMaze;
		mDifficulty = difficulty;
		mBestMaze.reset();
	}
}

void Generator::variate()
{
	double r = drnd();

	int x = irnd( mSize.x() );
	int y = irnd( mSize.y() );

	if( x<1 || x==mSize.x()-1 || y<1 || y==mSize.y()-1 ){}
	else if( r<mBorderExit )
	{
		if( r<0.25 ) x = 0;
		else if( r<0.5 ) x = mSize.x()-1;
		else if( r<0.75 ) y = 0;
		else y = mSize.y()-1;
	}

	mCurrentMaze.mExit.x() = x;
	mCurrentMaze.mExit.y() = y;

	mCurrentMaze.mStates.clear();

	State s;
	do{
		s.t.x() = irnd( mSize.x() );
		s.t.y() = irnd( mSize.y() );
	}while( s.t==mCurrentMaze.mExit );

	do{
		s.m.x() = irnd( mSize.x() );
		s.m.y() = irnd( mSize.y() );
	}while( s.t==s.m || s.m==mCurrentMaze.mExit );


	mCurrentMaze.mStates.push_back(s);
	mCurrentMaze.mIndex = 0;
}


void Generator::generateWalls()
{
	mCurrentMaze.mSize = mSize;
	mCurrentMaze.mWalls.clear();
	mCurrentMaze.mWalls.resize( mSize.xy(), Maze::Wall(0) );

	int sz = mSize.xy();
	for( int i=0; i<10*sz; i++ )
	{
		XY p;
		p.x() = irnd( mSize.x() );
		p.y() = irnd( mSize.y() );

	tryn:
		int side = irnd(4)+1;
		if( !mCurrentMaze.addWall(p,side) ) continue;
		int nwalls = mCurrentMaze.calcNumWalls();
		int mwalls = 2*mSize.xy()-mSize.x()-mSize.y();
		if( 1.0*nwalls/mwalls >= mFilling ) break;

		if( drnd() < mIsolation ) continue; // next wall

		double r = drnd();
		if( r < 0.25 ) --p.x();
		else if( r < 0.5 ) ++p.x();
		else if( r < 0.75 ) ++p.y();
		else --p.y();
		goto tryn;
	}
}

