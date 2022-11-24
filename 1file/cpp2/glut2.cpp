#include <GL/glut.h>

#include <iostream>

using namespace std;

void display(void)
{
   // clear all pixels
   glClear (GL_COLOR_BUFFER_BIT);

   // draw white polygon (rectangle) with corners at
   // (0.25, 0.25, 0.0) and (0.75, 0.75, 0.0)  
   glColor3f (1.0, 1.0, 1.0);
   glBegin(GL_POLYGON);
      glVertex3f (0.25, 0.25, 1.0);
      glVertex3f (0.75, 0.25, 0.0);
      glVertex3f (0.75, 0.75, 0.0);
      glVertex3f (0.25, 0.75, 0.0);
   glEnd();

   glFlush ();
}

void init (void) 
{
   // select clearing color 
   glClearColor (0.0, 0.0, 0.0, 0.0);

   // initialize viewing values
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}


void key(unsigned char k, int x, int y)
{
	cout<<"key="<<int(k)<<" x,y=("<<x<<","<<y<<")\n";
	if( k==27 ) exit(0);
}

void mouse(int button, int state, int x, int y)
{
	cout<<"mouse "<<button<<' '<<state<<' '<<x<<' '<<y<<' '<<'\n';
}

void move(int x, int y){ cout<<"move "<<x<<' '<<y<<' '<<'\n'; }
void shape(int x, int y){ cout<<"shape "<<x<<' '<<y<<' '<<'\n'; }
void winstat(int state){ cout<<"winstat "<<' '<<state<<'\n'; }

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize (250, 250); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("hello");
   init ();
   glutDisplayFunc(display); 
   cout<<"press Esc to exit\n";
   glutKeyboardFunc(key);
   glutMouseFunc(mouse);
   glutMotionFunc(move);
   glutWindowStatusFunc(winstat);
   glutReshapeFunc(shape);
   glutMainLoop();
}
