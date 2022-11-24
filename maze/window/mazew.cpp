// Theseus and Minotaur Game
// Oleg Mazonka 2010
// http://mazonka.com/maze/

#include <sstream>
#include <fstream>
#include <string>
#include <time.h>
#include <algorithm>

#include "wx/wx.h"
#include "wx/sizer.h"
#include "wx/dcbuffer.h"
#include "wx/dir.h"

#include "mazew.h"
#include "solver.h"
#include "generate.h"

const bool DBLBUFF=true;


BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_MENU(ID_Quit,  MyFrame::OnQuit)
	EVT_MENU(ID_About, MyFrame::OnAbout)
	EVT_MENU(ID_Keyboard, MyFrame::OnKeyboard)
	EVT_MENU(ID_Solve, MyFrame::OnSolve)
	EVT_MENU(ID_Ponder, MyFrame::OnPonder)
	EVT_MENU(ID_Save, MyFrame::OnSave)
	EVT_MENU(ID_Next, MyFrame::OnNext)
	EVT_MENU(ID_Prev, MyFrame::OnPrev)
	EVT_MENU(ID_Back, MyFrame::OnBack)
	EVT_MENU(ID_Forward, MyFrame::OnForward)
	EVT_MENU(ID_Reset, MyFrame::OnReset)
	EVT_MENU(ID_Gener, MyFrame::OnGener)
	EVT_MENU(ID_Options, MyFrame::OnOptions)
	END_EVENT_TABLE()

BEGIN_EVENT_TABLE(DrawPane, wxPanel)
	EVT_LEFT_DOWN(DrawPane::mouseDown)
	EVT_KEY_DOWN(DrawPane::keyPressed)
	EVT_PAINT(DrawPane::paintEvent)
	EVT_SIZE(DrawPane::OnSize)

	END_EVENT_TABLE()

BEGIN_EVENT_TABLE(BasicDialog, wxDialog)

	END_EVENT_TABLE()



	IMPLEMENT_APP(MyApp)


	bool MyApp::OnInit()
{
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	mFrame = new MyFrame( wxT("Theseus and Minotaur"), wxPoint(50,50), wxSize(800,600));

	mPane = new DrawPane( (wxFrame*) mFrame );
	mFrame->setPane(mPane);

	mFrame->loadMaze("",true);

	mPane->SetDoubleBuffered(DBLBUFF);

	sizer->Add(mPane, 1, wxEXPAND);

	mFrame->SetSizer(sizer);
	mFrame->SetAutoLayout(true);

	mFrame->Show(true);
	SetTopWindow(mFrame);

	mTimer = new RenderTimer(mPane);
	mTimer->start();

	return true;
}


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, -1, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;

	menuFile->Append( ID_Next, _("&Next maze\tCtrl-N") );
	menuFile->Append( ID_Prev, _("&Previous maze\tCtrl-P") );
	menuFile->AppendSeparator();
	menuFile->Append( ID_Save, _("&Save maze and solution") );
	menuFile->AppendSeparator();
	menuFile->Append( ID_Quit, _("E&xit\tCtrl-Q") );

	wxMenu *menuMaze = new wxMenu;

	menuMaze->Append( ID_Back,    _("&Back\tB") );
	menuMaze->Append( ID_Forward, _("&Forward\tF") );
	menuMaze->Append( ID_Reset,   _("&Try again\tR") );
	menuMaze->AppendSeparator();

	menuMaze->Append( ID_Ponder, _("&Ponder") );
	menuMaze->Append( ID_Solve, _("&Solve") );
	{
		std::ifstream in("nohelp");
		if( !!in )
		{
			menuMaze->Enable(ID_Ponder,false);
			menuMaze->Enable(ID_Solve,false);
		}
	}

	menuMaze->AppendSeparator();
	menuMaze->Append( ID_Options, _("&Generation options ...") );
	menuMaze->Append( ID_Gener, _("&Generate\tCtrl-G") );

	wxMenu *menuHelp = new wxMenu;

	menuHelp->Append( ID_Keyboard, _("&Keyboard") );
	menuHelp->AppendSeparator();
	menuHelp->Append( ID_About, _("&About...") );

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, _("&File") );
	menuBar->Append( menuMaze, _("&Maze") );
	menuBar->Append( menuHelp, _("&Help") );

	SetMenuBar( menuBar );

	CreateStatusBar();
	SetStatusText( wxString("Welcome to Theseus and Minotaur game; "
		"Try Help->Keyboard for keyboard commands",wxConvUTF8) );

}


void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxMessageBox( towx("This is the Theseus and Minotaur Game.\n"
		"Written on 23 Sept 2010\n"
		"http://mazonka.com/maze/"
		),
		_("Oleg Mazonka"), 
		wxOK | wxICON_INFORMATION, this );
}

void MyFrame::OnKeyboard(wxCommandEvent& WXUNUSED(event))
{
	wxMessageBox( towx("Arrows or mouse click - left/right/up/dpwn\n"
		"Space - wait for one move\n"
		"b - return back one move\n"
		"f - forward one move (if exists)\n"
		"r - reset - back all moves"),
		_("Game keyboard commands"), 
		wxOK | wxICON_INFORMATION, this );
}


DrawPane::DrawPane(wxFrame* parent) :
wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
	mMaze(0)
{
	mAnim=0;
	mFrame=parent;
}


void DrawPane::paintEvent(wxPaintEvent & evt)
{
	wxPaintDC dc(this);
	render(dc);
}

void DrawPane::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void DrawPane::OnSize(wxSizeEvent &e)
{
	Refresh();
}


void DrawPane::render(wxDC&  dc)
{
	dc.Clear();

	int w=0,h=0;
	dc.GetSize(&w,&h);
	dc.SetBrush(*wxMEDIUM_GREY_BRUSH);
	dc.SetPen( wxPen( wxColor(25,25,25), 1 ) );
	dc.DrawRectangle( 0, 0, w, h );

	if( !mMaze ) return;

	double xpos=0, ypos=0;
	double xsz = mMaze->size().x();
	double ysz = mMaze->size().y();

	if( w<10 || h<10 ) return;

	double xscale = xsz/w;
	double yscale = ysz/h;
	double scale = ( xscale > yscale ? xscale : yscale );
	scale /= 0.95;

	ysz /= scale;
	xsz /= scale;

	xpos = (w - xsz)/2;
	ypos = (h - ysz)/2;

	dc.SetBrush(*wxLIGHT_GREY_BRUSH);
	dc.SetPen( wxPen( wxColor(55,55,75), 1 ) );
	dc.DrawRectangle( xpos, ypos, xsz, ysz );

	drawMaze(dc,xpos,ypos,xsz,ysz);

}



void DrawPane::makeMove(int side)
{
	if( mAnim ) return;

	mSide = side;
	mAnim = 1;
	processAnim();
}

void DrawPane::processAnim()
{
	if( mAnim==0 ) return;

	if(mAnim==1)
	{
		mAnim=0;
		if( mMaze->moveThes(mSide) )
		{
			mAnim=2;
			Refresh();
		}
	}

	else if(mAnim==2)
	{
		mAnim=0;
		if( mMaze->moveMino() )
		{
			mAnim=3;
			Refresh();
		}
	}

	else if(mAnim==3)
	{
		mAnim=0;
		mMaze->moveMino();
		Refresh();
	}

	else if( mAnim==10 && mSolver)
	{
		mSolver->solve1();
		Refresh();
		if( mSolver->state() > 0 ) mAnim=12;
	}

	else if( mAnim==11 && mSolver)
	{
		mSolver->solveT(100);
		Refresh();
		if( mSolver->state() > 1 ) mAnim=12;
	}

	else if( mAnim==12 && mSolver)
	{
		mSolver->apply();
		if( mSolver->state() == 0 ) status( _("Solution not found") );
		if( mSolver->state() == 1 ) status( _("Some solution is found (use f)") );
		if( mSolver->state() == 2 ) status( _("Solution does not exist") );
		if( mSolver->state() == 3 ) statusSolution();
		delete mSolver;
		mAnim=0;
		Refresh();
		return;
	}

	else if( mAnim==20 && mGen )
	{
		mGen->generate(100);
		delete mMaze;
		mMaze = new Maze(mGen->getCurrentMaze());
		if( mGen->isDone() ) mAnim=21;
		int sol = mGen->getSolutionSize()-1;
		status( towx( "Generating "+tos(mGen->getCurrent())+" / "
			+tos(mGen->getTotal()) + " so far best " 
			+tos(sol) + " ("
			+tos(sol-mMaze->size().xy()) +")" ) );

		Refresh();
		return;
	}

	else if( mAnim==21 && mGen )
	{
		mAnim=0;
		delete mMaze;
		mMaze = new Maze(mGen->getBestMaze());
		int sol = mGen->getSolutionSize()-1;
		int diff = sol - mMaze->size().xy();
		if( sol < -1 )
		  status( towx("No solution for this maze!") );
		else
		  status( towx("Solution: "+tos(sol)+"  Difficulty: "+tos(diff)) );
		delete mGen;
		Refresh();
		return;
	}


	if( mAnim==0 )
	{
		if( mMaze->isLost() ) status( _("You lost!") );
		else if( mMaze->isWin() ) 
			status( towx("You win in "
			+tos(mMaze->getNumStates()-1)+" steps" ) );

		else statusCurrent();
	}
}

void DrawPane::keyPressed(wxKeyEvent& event)
{
	if( !mMaze ) return;

	if( mAnim==10 || mAnim==11 )
	{
		mAnim=12;
		return;
	}

	if( mAnim==20 )
	{
		mAnim=21;
		return;
	}

	std::ostringstream os;
	int code = event.GetKeyCode();
	os<<code;
	std::string s = "Code: "+os.str();

	if( WXK_SPACE==code ) makeMove(0);
	else if( WXK_RIGHT==code ) makeMove(1);
	else if( WXK_LEFT==code ) makeMove(2);
	else if( WXK_UP==code ) makeMove(3);
	else if( WXK_DOWN==code ) makeMove(4);

	statusCurrent();
	paintNow();
}

void DrawPane::loadMaze(std::istream &is)
{
	delete mMaze;
	mMaze = new Maze(is);
}



RenderTimer::RenderTimer(DrawPane* pane) : wxTimer()
{
	RenderTimer::pane = pane;
}

void RenderTimer::Notify()
{
	static bool in = false;
	if( in ) return;
	in = true;
	pane->processAnim();
	in = false;
}

void RenderTimer::start()
{
	wxTimer::Start(200);
}

void DrawPane::mouseDown(wxMouseEvent& e)
{
	std::ostringstream os;
	double x = e.GetX();
	double y = e.GetY();
	os<<x<<' '<<y;
	std::string s = "Mouse "+os.str();

	if( x>mSq.le && x<mSq.ri && y>mSq.to && y<mSq.bo ) makeMove(0);
	else if( x>mSq.le && x<mSq.ri+(mSq.ri-mSq.le) && y>mSq.to && y<mSq.bo ) makeMove(1);
	else if( x<mSq.le && x>mSq.le-(mSq.ri-mSq.le) && y>mSq.to && y<mSq.bo ) makeMove(2);
	else if( x>mSq.le && x<mSq.ri && y<mSq.to && y>mSq.to-(mSq.bo-mSq.to) ) makeMove(3);
	else if( x>mSq.le && x<mSq.ri && y>mSq.bo && y<mSq.bo+(mSq.bo-mSq.to) ) makeMove(4);
}


void MyFrame::OnSolve(wxCommandEvent& WXUNUSED(event))
{
	if( !mPane->getMaze() ) return;
	mPane->solve(11);
}

void MyFrame::OnPonder(wxCommandEvent& WXUNUSED(event))
{
	if( !mPane->getMaze() ) return;
	mPane->solve(10);
}

void DrawPane::status(wxString s)
{
	mFrame->SetStatusText( s );
}

void DrawPane::solve(int id)
{
	mSolver = new ShortSolver(mMaze);
	mAnim = id;
}

wxString towx(std::string s)
{
	wxString r(s.c_str(),wxConvUTF8);
	return r;
}

std::string wxto(wxString s)
{
	return (std::string)s.mb_str(wxConvUTF8);
}

wxString towx(int x)
{
	return towx(tos(x));
}

std::string tos(int x)
{
	std::ostringstream os;
	os<<x;
	return os.str();
}

int sto(std::string s)
{
	int x=-1;
	std::istringstream is(s);
	is>>x;
	return x;
}

int sto(wxString s)
{
	return sto( wxto(s) );
}

void DrawPane::statusSolution()
{
	wxString s1 = _("Found solution in ");
	wxString s2 = _(" steps (use f)");
	wxString s = s1+ towx(mMaze->getNumStates()-1) +s2;

	status( s );
}

void DrawPane::statusCurrent()
{
	status( towx(mMaze->getIndex()) + _(" / ") + towx(mMaze->getNumStates()-1) );
}

void MyFrame::loadMaze(const std::string &s, bool next)
{
	std::vector<std::string> files = getListOfFiles();
	if( files.empty() )
	{
		wxMessageBox( 
			_("No maze files found\nTry Maze->Generate menu item"), 
			_("Warning"), wxOK | wxICON_INFORMATION, NULL );
		return;
	}

	int i=0;
	int sz = files.size();
	for(  ; i<sz; i++ )
		if( files[i] == s ) break;

	if( i==sz )
		mFile = files[0];

	else if( next && i+1 < sz )
		mFile = files[i+1];

	else if( !next && i-1 >= 0 )
		mFile = files[i-1];

	std::ifstream fmaze(mFile.c_str());
	mPane->loadMaze(fmaze);

	if( !mPane->getMaze()->validate() )
	{
		wxMessageBox( towx("File "+mFile+" is corrupted"), _("ERROR"), wxOK | wxICON_INFORMATION, NULL );
		return;
	}

	SetStatusText( towx("Loaded " + mFile) );
}

std::vector<std::string> getListOfFiles()
{
	std::vector<std::string> r;

	wxDir dir(wxGetCwd());

	if ( !dir.IsOpened() )
		return r;

	wxString filename;

	bool cont = dir.GetFirst(&filename);
	while ( cont )
	{
		std::string s = wxto(filename);
		int sz = s.size();
		if( sz>5 && s.substr(sz-5)==".maze" )
			r.push_back(s);

		cont = dir.GetNext(&filename);
	}

	std::sort(r.begin(),r.end());

	return r;
}


void MyFrame::OnNext(wxCommandEvent& WXUNUSED(event))
{
	loadMaze(mFile,true);
	Refresh();
}

void MyFrame::OnPrev(wxCommandEvent& WXUNUSED(event))
{
	loadMaze(mFile,false);
	Refresh();
}

void MyFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
	if( !mPane->getMaze() ) return;
	std::ofstream of(mFile.c_str());
	mPane->getMaze()->save(of);
}


void MyFrame::OnBack(wxCommandEvent& WXUNUSED(event))
{
	if( !mPane->getMaze() ) return;
	mPane->getMaze()->back();
	Refresh();
}

void MyFrame::OnForward(wxCommandEvent& WXUNUSED(event))
{
	if( !mPane->getMaze() ) return;
	mPane->getMaze()->forward();
	Refresh();
}

void MyFrame::OnReset(wxCommandEvent& WXUNUSED(event))
{
	if( !mPane->getMaze() ) return;
	mPane->getMaze()->reset();
	Refresh();
}


BasicDialog::BasicDialog ( wxWindow * parent, wxWindowID id, const wxString & title,
                           const wxPoint & position, const wxSize & size, long style )
: wxDialog( parent, id, title, position, size, style)
{

	SetSize(size);
	wxPoint p;

	int NL = 10;
 
	p.x = NL; p.y = 20;

	new wxStaticText( this, -1, _("Maze name"), p );
	p.x += 70;
	mName = new wxTextCtrl ( this, -1, towx("o")+towx(time(0)), p );

	p.x += 130;
	new wxStaticText( this, -1, _("Size"), p );
	p.x += 30;
	mX = new wxTextCtrl ( this, -1, towx("1"), p, wxSize(40,-1) );
	p.x += 45;
	new wxStaticText( this, -1, _("X"), p );
	p.x += 15;
	mY = new wxTextCtrl ( this, -1, towx("1"), p, wxSize(40,-1) );

	p.x = NL;
	p.y += 35;
	new wxStaticText( this, -1, _("Wall filling"), p );
	p.x += 70;
	new wxStaticText( this, -1, _("Sparse"), p );
	p.x += 35;
	mFill = new wxSlider( this, -1, 200, 0, 1000, p, wxSize(200,-1) );
	p.x += 200;
	new wxStaticText( this, -1, _("Dense"), p );

	p.x = NL;
	p.y += 30;
	new wxStaticText( this, -1, _("Isolation"), p );
	p.x += 70;
	new wxStaticText( this, -1, _("Small"), p );
	p.x += 35;
	mIsol = new wxSlider( this, -1, 200, 0, 1000, p, wxSize(200,-1) );
	p.x += 200;
	new wxStaticText( this, -1, _("Big"), p );

	p.x = NL;
	p.y += 30;
	new wxStaticText( this, -1, _("Border exit"), p );
	p.x += 70;
	new wxStaticText( this, -1, _("   No"), p );
	p.x += 35;
	mExit = new wxSlider( this, -1, 200, 0, 1000, p, wxSize(200,-1) );
	p.x += 200;
	new wxStaticText( this, -1, _("Yes"), p );

	p.x = NL;
	p.y += 30;
	new wxStaticText( this, -1, _("Use walls"), p );
	p.x += 70;
	new wxStaticText( this, -1, _("  New"), p );
	p.x += 35;
	mVary = new wxSlider( this, -1, 200, 0, 1000, p, wxSize(200,-1) );
	p.x += 200;
	new wxStaticText( this, -1, _("Keep"), p );

	p.x = NL;
	p.y += 40;
	new wxStaticText( this, -1, _("Generate"), p );
	p.x += 70;
	mNum = new wxTextCtrl ( this, -1, towx("0"), p, wxSize(60,-1) );
	p.x += 70;
	new wxStaticText( this, -1, _(" mazes and select the most difficult"), p );

	p.y = size.GetHeight() - 60;
	p.x = size.GetWidth() - 100;
	wxButton * b = new wxButton( this, wxID_OK, _("OK"), p, wxDefaultSize );
	p.x -= 100;
	wxButton * c = new wxButton( this, wxID_CANCEL, _("Cancel"), p, wxDefaultSize );
}


void DrawPane::setGenerator(const Generator &g)
{
	if( mMaze ) mGen = new Generator(g,*mMaze);
	else mGen = new Generator(g);
	mAnim = 20;
}


void MyFrame::OnOptions(wxCommandEvent& WXUNUSED(event))
{
	BasicDialog dia ( this, -1, _("Generate options"),
		wxDefaultPosition, wxSize(400, 300) );

	dia.mX->SetValue( towx(mGen.getSizeX()) );
	dia.mY->SetValue( towx(mGen.getSizeY()) );

	dia.mFill->SetValue( int(1000*mGen.getFilling()) );
	dia.mIsol->SetValue( int(1000*mGen.getIsolation()) );
	dia.mExit->SetValue( int(1000*mGen.getBorderExit()) );
	dia.mVary->SetValue( int(1000*mGen.getVariator()) );
	dia.mNum->SetValue( towx(mGen.getTotal()) );

	if ( dia.ShowModal() != wxID_OK ) return;

	mGen.name = wxto(dia.mName->GetValue());

	int sizex = sto( dia.mX->GetValue() );
	if( sizex<3 || sizex>10000 ) sizex=3;
	int sizey = sto( dia.mY->GetValue() );
	if( sizey<3 || sizey>10000 ) sizey=3;

	int num = sto( dia.mNum->GetValue() );
	if( num < 1 ) num=1;

	double fil = dia.mFill->GetValue()/1000.0;
	double iso = dia.mIsol->GetValue()/1000.0;
	double exi = dia.mExit->GetValue()/1000.0;
	double var = dia.mVary->GetValue()/1000.0;

	mGen.set( XY(sizex,sizey), num, fil, iso, exi, var );

	mFile = mGen.name+".maze";
}

void MyFrame::OnGener(wxCommandEvent& WXUNUSED(event))
{
	if( mGen.name=="" ) mGen.name = "o"+tos(time(0));
	mPane->setGenerator(mGen);
	mFile = mGen.name+".maze";

	SetStatusText( towx("New maze "+mFile+" being generated (not saved)"));
}


