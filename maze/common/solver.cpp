// Theseus and Minotaur Game
// Oleg Mazonka 2010
// http://mazonka.com/maze/

#include <time.h>

#include "solver.h"

const bool VAL=false;

int Solver::state() const
{
	if( mMaze->size().x()<1 ) return 0;

	if( !mBest.empty() )
	{
		if( mExhausted ) return 3;
		return 1;
	}
	if( mExhausted ) return 2;
	return 0;
}

void Solver::solveT(int n)
{
	double start = clock();
	while(1)
	{
		solve1();
		if( mExhausted ) break;

		double passed = clock();

		if( n>=0 && (passed-start)*1000/CLOCKS_PER_SEC > n ) break;
	}
}


void Solver::solve1()
{
	if( mMaze->size().x()<1 ) return;

	mVisited.insert(mMaze->getState());

	int m=0;

trym:
	for( ; m<5; m++ )
	{
		if( !mMaze->moveThes(m) ) continue;
		mMaze->moveMino();
		mMaze->moveMino();

		if( mVisited.find( mMaze->getState() ) != mVisited.end() 
			|| mMaze->isLost() )
		{
			mMaze->back(); 
			continue;
		}

		if( mMaze->isWin() )
		{
			if( mBest.empty() || mBest.size()-1 > mMoves.size() )
			{
				mBest = mMoves;
				mBest.push_back(m);
			}

			mMaze->back(); 
			continue;
		}

		mMoves.push_back(m);
		break;
	}

	if( m==5 )
	{
		if( mMoves.empty() )
		{
			mExhausted = true;
			return;
		}
		m = mMoves.back()+1;
		mMoves.pop_back();
		mMaze->back();
		goto trym;
	}
}

void Solver::apply()
{
	if( mMaze->size().x()<1 ) return;

	while( !mMoves.empty() )
	{
		mMaze->back();
		mMoves.pop_back();
	}

	int index = mMaze->getIndex();

	for( int i=0; i<(int)mBest.size(); i++ )
	{
		mMaze->moveThes(mBest[i]);
		mMaze->moveMino();
		mMaze->moveMino();
	}       

	mMaze->setIndex(index);
}


// SHORTSOLVER ******************

void ShortSolver::solve1()
{
	if( mMaze->size().x()<1 ) return;

	State thisState = mMaze->getState();
	mGraph[thisState] = value();
	if(VAL) validate();

	int m=4;

trym:
	for( ; m>=0; m-- )
	{
		if( !mMaze->moveThes(m) ) continue;
		mMaze->moveMino();
		mMaze->moveMino();
		State newState = mMaze->getState();

		if( mMaze->isLost() )
		{
			mMaze->back(); 
			continue;
		}

		int cdepth = mMoves.size()+1; // extra for m

		GraphType::iterator gi = mGraph.find( newState );
		if( gi != mGraph.end() )
		{
			int gdepth = gi->second.depth;
			if( cdepth < gdepth )
			{
				gi->second.state = thisState;
				gi->second.depth = cdepth;
				if(VAL) validate();
			}
			else
			{
				mMaze->back(); 
				continue;
			}
		}

		if( mMaze->isWin() )
		{
			mFinal.insert(newState);
			mGraph[newState].state = thisState;
			mGraph[newState].depth = cdepth;
			if(VAL) validate();

			mMaze->back(); 
			continue;
		}

		mPrev = thisState;
		mMoves.push_back(m);
		break;
	}

	if( m<0 )
	{
		if( mMoves.empty() )
		{
			mExhausted = true;
			return;
		}
		m = mMoves.back()-1;
		mMoves.pop_back();
		mMaze->back();
		thisState = mMaze->getState();
		goto trym;
	}
}

void ShortSolver::apply()
{
	if( mFinal.empty() ){ mMaze->cut(); return; }

	if( mMaze->size().x()<1 ) return;

	while( !mMoves.empty() )
	{
		mMaze->back();
		mMoves.pop_back();
	}

	int index = mMaze->getIndex();

	// find shortest path
	State shortest;
	int depth=-1;

	for( std::set<State>::const_iterator j=mFinal.begin(); j!=mFinal.end(); ++j )
	{
		State s = *j;

		int i=0;
		for( ; i<(int)mGraph.size(); i++)
		{
			s = mGraph[s].state;
			if( s == State() ) break;
		}

		if( depth==-1 || i<depth )
		{
			shortest = *j;
			depth = i;
		}
	}


	State current = shortest;


	State mazeState = mMaze->getState();
	std::vector<State> vs;

	for( int i=0; i<(int)mGraph.size(); i++)
	{
		vs.push_back(current);
		current = mGraph[current].state;
		if( current == mazeState ) break;
		if( current == State() )
		{
			break; // error
		}
	}

	for( int i=vs.size()-1; i>=0; i-- )
	{
		mMaze->moveTo(vs[i]);
	}       

	mMaze->setIndex(index);
}

void ShortSolver::validate()
{
	for( GraphType::iterator i = mGraph.begin(); i!=mGraph.end(); ++i )
	{
		State k = i->first;
		State v = i->second.state;
		if( v==State() ) return;
		int s = 0, a;
		a = k.t.x()-v.t.x(); s += a*a;
		a = k.t.y()-v.t.y(); s += a*a;

		if( s>1 )
		{
			break; //error
		}
	}
}

int ShortSolver::state() const
{
	if( mMaze->size().x()<1 ) return 0;

	if( !mFinal.empty() )
	{
		if( mExhausted ) return 3;
		return 1;
	}
	if( mExhausted ) return 2;
	return 0;
}
