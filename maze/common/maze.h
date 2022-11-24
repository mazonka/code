// Theseus and Minotaur Game
// Oleg Mazonka 2010
// http://mazonka.com/maze/

#ifndef MAZE_H
#define MAZE_H

#include <istream>
#include <ostream>
#include <vector>

class Generator;

class XY
{
	int mX,mY;
public:
	XY(): mX(-1), mY(-1) {}
	XY(int x, int y): mX(x), mY(y) {}

	int x() const { return mX; }
	int y() const { return mY; }
	int& y() { return mY; }
	int& x() { return mX; }
	int xy() const { return mX*mY; }

	bool operator==(const XY& a) const { return mX==a.mX && mY==a.mY; }
	bool operator!=(const XY& a) const { return !(a==*this); }
	bool operator<(const XY &s) const { return mX<s.mX?true:s.mX<mX?false:mY<s.mY; }

	friend std::istream & operator>>(std::istream &is, XY& xy)
	{ return is>>xy.mX>>xy.mY; }

	friend std::ostream & operator<<(std::ostream &os, const XY& xy)
	{ return os<<xy.mX<<' '<<xy.mY; }
};

struct State
{
	XY t,m;
	State(){}
	State(XY a, XY b): t(a), m(b) {}
	bool operator<(const State &s) const { return t<s.t?true:s.t<t?false:m<s.m; }
	bool operator==(const State& a) const { return t==a.t && m==a.m; }

	friend std::istream & operator>>(std::istream &is, State& s)
	{ return is>>s.t>>s.m; }

	friend std::ostream & operator<<(std::ostream &os, const State& s)
	{ return os<<s.t<<"  "<<s.m; }
};

class Box
{
protected:
	enum Wall { none=0, right=1, bottom=2, both=3 };
	XY mSize;
	std::vector<Wall> mWalls;

	Box(){}

private:
	bool checkTopology(XY p);
	void toporec(std::vector<int> &v, XY p);

public:
	XY size() const { return mSize; }

	int idx(int i, int j) const { return j*mSize.x()+i; }
	int idx(XY p) const { return idx(p.x(),p.y()); }
	Wall wall(int i, int j) const { return mWalls[idx(i,j)]; }
	bool canmove(int i, int j, int side);
	bool canmove(XY p, int side){ return canmove(p.x(), p.y(), side); }

	bool hasRight(int i, int j) const { return 0!=(wall(i,j)&right); }
	bool hasBottom(int i, int j) const { return 0!=(wall(i,j)&bottom); }

	int calcNumWalls() const;
	bool addWall(XY p, int side);
};

class Maze : public Box
{
	enum Solved { notsolved=0, user=1, comp=2 };

	XY mExit;
	std::vector<State> mStates;
	int mIndex;
	Solved mSolved;


public:
	Maze(){}
	Maze(std::istream &is);
	void save(std::ostream &of);

	XY exit() const { return mExit; }
	XY thes() const { return mStates[mIndex].t; }
	XY mino() const { return mStates[mIndex].m; }
	State getState() const { return mStates[mIndex]; }
	int getIndex() const { return mIndex; }
	void setIndex(int i){ mIndex = i; }
	int getNumStates() const { return mStates.size(); }

	bool moveTo(State s);
	bool moveThes(int side);
	bool moveMino();

	void reset(){ mIndex=0; cut(); }
	void back(){ if(--mIndex<0) mIndex=0; }
	void forward(){ if(++mIndex==getNumStates()) back(); }

	bool isLost() const { return thes() == mino(); }
	bool isWin() const { return thes() == exit(); }

	bool validate();
	void cut();

	friend class Generator;
};


#endif
