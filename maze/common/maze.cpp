// Theseus and Minotaur Game
// Oleg Mazonka 2010
// http://mazonka.com/maze/

#include "maze.h"

Maze::Maze(std::istream &is)
{
	is>>mSize;
	is>>mExit;

	int sz = mSize.xy();
	for( int i=0; i<sz; i++ )
	{
		int w=-1;
		is>>w;
		mWalls.push_back( Wall(w) );
	}

	int solved=0;
	is>>solved;
	mSolved = Solved(solved);

	mIndex = 0;

	int histSz=0;
	is>>histSz;

	for( int i=0; i<histSz; i++ )
	{
		State s;
		is>>s;
		mStates.push_back(s);
	}
}


void Maze::save(std::ostream &of)
{
	of<<mSize<<'\n';
	of<<mExit<<'\n';

	for( int j=0; j<mSize.y(); j++ )
	{
	for( int i=0; i<mSize.x(); i++ )
	  {
		i && of<<' ';
		of<<int( wall(i,j) );
	  }
	  of<<'\n';
	}

	of<<int(mSolved)<<'\n';

	of<<mStates.size()<<'\n';

	for( int i=0; i<(int)mStates.size(); i++ )
	{
		of<<mStates[i]<<'\n';
	}

	of<<'\n';
}

bool Box::canmove(int i, int j, int side)
{
	if( i==0 && side==2 ) return false;
	if( i==mSize.x()-1 && side==1 ) return false;
	if( j==0 && side==3 ) return false;
	if( j==mSize.y()-1 && side==4 ) return false;

	if( side==1 && hasRight(i,j) ) return false;
	if( side==2 && hasRight(i-1,j) ) return false;
	if( side==4 && hasBottom(i,j) ) return false;
	if( side==3 && hasBottom(i,j-1) ) return false;

	return true;
}


bool Maze::moveThes(int side)
{
	int i = mStates[mIndex].t.x();
	int j = mStates[mIndex].t.y();

	if( !canmove(i,j,side) ) return false;

	mSolved = notsolved;
	mStates.resize(++mIndex);

	mStates.push_back( mStates.back() );

	if( side==1 ) mStates.back().t.x()++;
	if( side==2 ) mStates.back().t.x()--;
	if( side==3 ) mStates.back().t.y()--;
	if( side==4 ) mStates.back().t.y()++;

	return true;
}


bool Maze::moveMino()
{
	int ti = mStates[mIndex].t.x();
	int tj = mStates[mIndex].t.y();
	int mi = mStates[mIndex].m.x();
	int mj = mStates[mIndex].m.y();

	if( ti<mi && canmove(mi,mj,2) ) mStates.back().m.x()--;
	else if( ti>mi && canmove(mi,mj,1) ) mStates.back().m.x()++;
	else if( tj<mj && canmove(mi,mj,3) ) mStates.back().m.y()--;
	else if( tj>mj && canmove(mi,mj,4) ) mStates.back().m.y()++;
	else return false;

	return true;
}

bool Maze::moveTo(State s)
{
	mSolved = notsolved;
	mStates.resize(++mIndex);

	mStates.push_back( s );
	return true;
}

bool Maze::validate()
{
	if( mStates.empty() ) goto bad;
	if( mSize.x() < 2 || mSize.y() < 2 ) goto bad;

	return true;

bad:
	mSize.x() = -1;
	mSize.y() = -1;
	return false;
}

void Maze::cut()
{
	mStates.resize(mIndex+1);
}

int Box::calcNumWalls() const
{
	int sum=0;

	for( int i=0; i<(int)mWalls.size(); i++ )
	{
		if( mWalls[i]==3 ) sum+=2;
		else if( mWalls[i] ) ++sum;
	}

	return sum;
}

bool Box::addWall(XY p, int side)
{
	if( p.x()<0 || p.x()>=mSize.x() ) return false;
	if( p.y()<0 || p.y()>=mSize.y() ) return false;

	if( side==2 ) return addWall( XY(p.x()-1,p.y()), 1);
	if( side==3 ) return addWall( XY(p.x(),p.y()-1), 4);

	Wall &w = mWalls[p.y()*mSize.x()+p.x()];

	if( side==1 && p.x()==mSize.x()-1 ) return false;
	if( side==4 && p.y()==mSize.y()-1 ) return false;

	int bit = side;
	if( side==4 ) bit = 2;

	w = Wall( w|bit );

	XY n = p;
	++n.x();
	if( side==4 ){ --n.x(); ++n.y(); }

	if( !checkTopology(p) || !checkTopology(n) )
	{
		w = Wall( w&~bit );
		return false;
	}

	return true;
}

bool Box::checkTopology(XY p)
{
	std::vector<int> v(mSize.xy(),0);

	toporec(v,p);

	for( int i=0; i<(int)v.size(); i++ ) if( v[i]==0 ) return false;

	return true;
}

void Box::toporec(std::vector<int> &v, XY p)
{
	v[ idx(p) ] = 1;

	XY n;

	if( canmove(p,1) ){ n=p; ++n.x(); if( !v[idx(n)] ) toporec(v,n); }
	if( canmove(p,2) ){ n=p; --n.x(); if( !v[idx(n)] ) toporec(v,n); }
	if( canmove(p,3) ){ n=p; --n.y(); if( !v[idx(n)] ) toporec(v,n); }
	if( canmove(p,4) ){ n=p; ++n.y(); if( !v[idx(n)] ) toporec(v,n); }
}

