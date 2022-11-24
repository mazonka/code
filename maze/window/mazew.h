// Theseus and Minotaur Game
// Oleg Mazonka 2010
// http://mazonka.com/maze/

#include <sstream>
#include <fstream>
#include <string>

#include "wx/wx.h"
#include "wx/sizer.h"

#include "maze.h"
#include "generate.h"

class DrawPane;
class Solver;
class Generator;

class MyFrame: public wxFrame
{
    DrawPane *mPane;
    std::string mFile;
    Generator mGen;

public:

    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    void setPane(DrawPane *p){ mPane=p; }

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnKeyboard(wxCommandEvent& event);
    void OnSolve(wxCommandEvent& event);
    void OnPonder(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnNext(wxCommandEvent& event);
    void OnPrev(wxCommandEvent& event);
    void OnBack(wxCommandEvent& event);
    void OnForward(wxCommandEvent& event);
    void OnReset(wxCommandEvent& event);
    void OnGener(wxCommandEvent& event);
    void OnOptions(wxCommandEvent& event);

    void loadMaze(const std::string& file, bool next);
    void saveMaze();

    DECLARE_EVENT_TABLE()
};

class DrawPane : public wxPanel
{
    Maze * mMaze;
    wxFrame* mFrame;

    int mSide, mAnim;
    Solver *mSolver;
    Generator *mGen;

public:
    struct Square{ double le,ri,to,bo; };

private:
    Square mSq;

public:
    DrawPane(wxFrame* parent);
    void loadMaze(std::istream &is);

    void status(wxString s);
    void statusSolution();
    void statusCurrent();
    void paintEvent(wxPaintEvent & evt);
    void paintNow();
    
    void render(wxDC& dc);
    void drawMaze(wxDC& dc, double xpos, double ypos, double w, double h);
    void makeMove(int side);
    Maze * getMaze() const { return mMaze; }
    void solve(int id);
    void setGenerator(const Generator &g);
    
     void mouseDown(wxMouseEvent& event);

     void keyPressed(wxKeyEvent& event);
     void processAnim();

     void OnSize(wxSizeEvent &e);
    
    DECLARE_EVENT_TABLE()
};

class RenderTimer : public wxTimer
{
    DrawPane* pane;
public:
    RenderTimer(DrawPane* pane);
    void Notify();
    void start();
};


class MyApp: public wxApp
{
    virtual bool OnInit();

    MyFrame *mFrame;
    DrawPane *mPane;
    RenderTimer* mTimer;
};


class BasicDialog: public wxDialog
{
public:
 
	BasicDialog ( wxWindow * parent, wxWindowID id, const wxString & title,
	              const wxPoint & pos = wxDefaultPosition,
	              const wxSize & size = wxDefaultSize,
	              long style = wxDEFAULT_DIALOG_STYLE );
 
	wxTextCtrl * mName;
	wxTextCtrl * mX , * mY;
	wxTextCtrl * mNum;

	wxSlider * mFill;
	wxSlider * mIsol;
	wxSlider * mExit;
	wxSlider * mVary;
	
	wxString GetText();
 
private:
 
	void OnOk( wxCommandEvent & event );
 
	DECLARE_EVENT_TABLE()
};

enum
{
    ID_Quit = 100,
    ID_About,
    ID_Keyboard,
    ID_Back,
    ID_Forward,
    ID_Reset,
    ID_Ponder,
    ID_Solve,
    ID_Save,
    ID_Next,
    ID_Prev,
    ID_Options,
    ID_Gener
};

wxString towx(int x);
wxString towx(std::string s);
std::string wxto(wxString s);
std::string tos(int x);
int sto(std::string s);
int sto(wxString s);

std::vector<std::string> getListOfFiles();
