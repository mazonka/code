// Theseus and Minotaur Game
// Oleg Mazonka 2010
// http://mazonka.com/maze/

#ifndef GENERATE_H
#define GENERATE_H

#include <string>

#include "maze.h"


class Generator
{
	XY mSize;

	Maze mCurrentMaze;
	bool mCurrentSet;
	Maze mBestMaze;
	int mDifficulty;
	int mTotal, mCurrent;

	double mFilling;
	double mIsolation;
	double mBorderExit;
	double mVariator;

	void generate1();
	void generateWalls();
	void variate();

public:
	std::string name;

public:
	Generator();
	Generator(const Generator& g, const Maze &m);

	void generate(int n);
	Maze getCurrentMaze(){ return mCurrentMaze; }
	Maze getBestMaze(){ return mBestMaze; }
	bool isDone() const{ return mCurrent>=mTotal; }

	void set(XY s, int n, double f, double i, double b, double v)
	{
		mSize = s;
		mTotal = n;
		mFilling = f;
		mIsolation = i;
		mBorderExit = b;
		mVariator = v;
	}

	int getSizeX() const { return mSize.x(); }
	int getSizeY() const { return mSize.y(); }
	double getFilling() const { return mFilling; }
	double getIsolation() const { return mIsolation; }
	double getBorderExit() const { return mBorderExit; }
	double getVariator() const { return mVariator; }
	int getSolutionSize() const { return mDifficulty; }
	int getTotal() const { return mTotal; }
	int getCurrent() const { return mCurrent; }
};


#endif
