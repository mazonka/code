// Theseus and Minotaur Game
// Oleg Mazonka 2010
// http://mazonka.com/maze/

#ifndef SOLVER_H
#define SOLVER_H

#include <vector>
#include <set>
#include <map>

#include "maze.h"

class Solver
{
protected:
	Maze * mMaze;

private:
	std::vector<int> mBest;

protected:
	std::vector<int> mMoves;
	std::set<State> mVisited;

	bool mExhausted;

public:
	Solver(Maze *m) : mMaze(m), mExhausted(false) {}
	virtual void solve1();
	void solveT(int n);
	virtual int state() const;
	virtual void apply();

};

class ShortSolver : public Solver
{
	struct Value { int depth; State state; };
	typedef std::map<State,Value> GraphType;
	GraphType mGraph;
	State mPrev;
	std::set<State> mFinal;

	inline Value value();

public:
	ShortSolver(Maze *m): Solver(m) {}
	virtual void solve1();
	virtual void apply();
	virtual int state() const;

	void validate();
};

inline ShortSolver::Value ShortSolver::value()
{
	Value v;
	v.depth = mMoves.size();
	v.state = mPrev;
	return v;
}


#endif
