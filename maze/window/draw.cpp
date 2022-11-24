// Theseus and Minotaur Game
// Oleg Mazonka 2010
// http://mazonka.com/maze/

#include "mazew.h"

struct Drawer
{
	wxDC* mDc;
	double mXpos, mYpos;
	double mW, mH;
	Maze * mMaze;

	double mCellSize;
	double mXoff, mYoff;

	void init();

	void cell(int i, int j);
	void exit();
	DrawPane::Square thes();
	void mino();
};


void DrawPane::drawMaze(wxDC& dc, double xpos, double ypos, double w, double h)
{
	int szx = mMaze->size().x();
	int szy = mMaze->size().y();
	if( szx<1 || szy<1 ) return;

	Drawer drawer;
	drawer.mDc = &dc;
	drawer.mXpos = xpos;
	drawer.mYpos = ypos;
	drawer.mW = w;
	drawer.mH = h;
	drawer.mMaze = mMaze;

	drawer.init();

	for( int j=0; j<szy; j++ )
		for( int i=0; i<szx; i++ )
			drawer.cell(i,j);

	drawer.exit();
	mSq = drawer.thes();
	drawer.mino();
}

void Drawer::init()
{
	mCellSize = mW*0.9/mMaze->size().x();

	mXoff = (mW - mMaze->size().x()*mCellSize)/2;
	mXoff += mCellSize/2;

	mYoff = (mH - mMaze->size().y()*mCellSize)/2;
	mYoff += mCellSize/2;
}

void Drawer::cell(int i, int j)
{
	double cx = mXoff+mCellSize*i;
	double cy = mYoff+mCellSize*j;

	double le = cx-mCellSize/2;
	double ri = cx+mCellSize/2;
	double to = cy-mCellSize/2;
	double bo = cy+mCellSize/2;

	int borderSize = mCellSize/30+1;
	int wallSize = mCellSize/40+1;


	mDc->SetPen( wxPen( wxColor(180,180,180), 1 ) );
	mDc->DrawLine( mXpos+le, mYpos+bo, mXpos+ri, mYpos+bo );
	mDc->DrawLine( mXpos+ri, mYpos+to, mXpos+ri, mYpos+bo );


	// left wall
	if( i==0 )
	{
		mDc->SetPen( wxPen( wxColor(0,0,0), borderSize ) );
		if( i==mMaze->exit().x() && j==mMaze->exit().y() )
		{
			mDc->DrawLine( mXpos+le, mYpos+to, 
				mXpos+le-mCellSize/8, mYpos+to );
			mDc->DrawLine( mXpos+le, mYpos+bo, 
				mXpos+le-mCellSize/8, mYpos+bo );
		}
		else
		{
			mDc->DrawLine( mXpos+le, mYpos+to, mXpos+le, mYpos+bo );
		}
	}

	// top wall
	if( j==0 )
	{
		mDc->SetPen( wxPen( wxColor(0,0,0), borderSize ) );
		if( i==mMaze->exit().x() && j==mMaze->exit().y() )
		{
			mDc->DrawLine( mXpos+le, mYpos+to, 
				mXpos+le, mYpos+to-mCellSize/8 );
			mDc->DrawLine( mXpos+ri, mYpos+to, 
				mXpos+ri, mYpos+to-mCellSize/8 );
		}
		else
		{
			mDc->DrawLine( mXpos+le, mYpos+to, mXpos+ri, mYpos+to );
		}
	}

	// right
	if( i==mMaze->size().x()-1 )
	{
		mDc->SetPen( wxPen( wxColor(0,0,0), borderSize ) );
		if( i==mMaze->exit().x() && j==mMaze->exit().y() )
		{
			mDc->DrawLine( mXpos+ri, mYpos+to, 
				mXpos+ri+mCellSize/8, mYpos+to );
			mDc->DrawLine( mXpos+ri, mYpos+bo, 
				mXpos+ri+mCellSize/8, mYpos+bo );
		}
		else
		{
			mDc->DrawLine( mXpos+ri, mYpos+to, mXpos+ri, mYpos+bo );
		}
	}
	else
	{
		if( mMaze->hasRight(i,j) )
		{
			mDc->SetPen( wxPen( wxColor(0,0,0), wallSize ) );
			mDc->DrawLine( mXpos+ri, mYpos+to, mXpos+ri, mYpos+bo );
		}
	}

	// bottom
	if( j==mMaze->size().y()-1 )
	{
		mDc->SetPen( wxPen( wxColor(0,0,0), borderSize ) );
		if( i==mMaze->exit().x() && j==mMaze->exit().y() )
		{
			mDc->DrawLine( mXpos+le, mYpos+bo, 
				mXpos+le, mYpos+bo+mCellSize/8 );
			mDc->DrawLine( mXpos+ri, mYpos+bo, 
				mXpos+ri, mYpos+bo+mCellSize/8 );
		}
		else
		{
			mDc->DrawLine( mXpos+le, mYpos+bo, mXpos+ri, mYpos+bo );
		}
	}
	else
	{
		if( mMaze->hasBottom(i,j) )
		{
			mDc->SetPen( wxPen( wxColor(0,0,0), wallSize ) );
			mDc->DrawLine( mXpos+le, mYpos+bo, mXpos+ri, mYpos+bo );
		}
	}
}

void Drawer::exit()
{
	int i = mMaze->exit().x();
	int j = mMaze->exit().y();
	if( i!=0 && i!=mMaze->size().x()-1 && j!=0 && j!=mMaze->size().y()-1 )

	{
		double cx = mXoff+mCellSize*i;
		double cy = mYoff+mCellSize*j;

		double le = cx-mCellSize/3;
		double ri = cx+mCellSize/3;
		double to = cy-mCellSize/3;
		double bo = cy+mCellSize/3;

		int size = mCellSize/20+1;

		mDc->SetPen( wxPen( wxColor(80,180,80), size ) );
		mDc->DrawLine( mXpos+le, mYpos+to, mXpos+ri, mYpos+to );
		mDc->DrawLine( mXpos+le, mYpos+to, mXpos+le, mYpos+bo );
		mDc->DrawLine( mXpos+le, mYpos+bo, mXpos+ri, mYpos+bo );
		mDc->DrawLine( mXpos+ri, mYpos+to, mXpos+ri, mYpos+bo );
	}
}

DrawPane::Square Drawer::thes()
{
	int ei = mMaze->exit().x();
	int ej = mMaze->exit().y();

	int i = mMaze->thes().x();
	int j = mMaze->thes().y();

	double cx = mXoff+mCellSize*i;
	double cy = mYoff+mCellSize*j;

	double le = cx-mCellSize/2;
	double ri = cx+mCellSize/2;
	double to = cy-mCellSize/2;
	double bo = cy+mCellSize/2;

	int size = mCellSize/10+1;

	if( ei==i && ej==j ) mDc->SetBrush(*wxGREEN_BRUSH);
	mDc->SetPen( wxPen( wxColor(0,0,0), size/3 ) );
	mDc->DrawCircle( wxPoint(mXpos+cx,mYpos+cy), 3*size );

	DrawPane::Square r;
	r.le = mXpos+le;
	r.ri = mXpos+ri;
	r.to = mYpos+to;
	r.bo = mYpos+bo;
	return r;
}

void Drawer::mino()
{
	int ti = mMaze->thes().x();
	int tj = mMaze->thes().y();

	int i = mMaze->mino().x();
	int j = mMaze->mino().y();

	double cx = mXoff+mCellSize*i;
	double cy = mYoff+mCellSize*j;

	double le = cx-mCellSize/3;
	double ri = cx+mCellSize/3;
	double to = cy-mCellSize/3;
	double bo = cy+mCellSize/3;

	int size = mCellSize/8+1;

	if( i==ti && j==tj ) 
		mDc->SetBrush(*wxRED_BRUSH);

	mDc->SetPen( wxPen( wxColor(0,0,0), size/3 ) );
	mDc->DrawCircle( wxPoint(mXpos+cx,mYpos+cy), 2*size );

	mDc->DrawLine( mXpos+le, mYpos+to, mXpos+le+size, mYpos+to+size );
	mDc->DrawLine( mXpos+ri, mYpos+to, mXpos+ri-size, mYpos+to+size );
	mDc->DrawLine( mXpos+le, mYpos+bo, mXpos+le+size, mYpos+bo-size );
	mDc->DrawLine( mXpos+ri, mYpos+bo, mXpos+ri-size, mYpos+bo-size );
}

