/*
 * Simple program to demonstrate generating coordinates
 * using the Lorenz Attractor
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/*  Lorenz Parameters  */
double s_  = 10;
double b_  = 2.6666;
double r_  = 28;

const int num_points_ = 50000;
int th_ = 0.0; // azimuth angle
int ph_ = 0.0; // elevation angle
double points_[50000][3]; // points for lorenz attractor graph
GLfloat view_z_ = 60.0;

double dim_ = 2.0;

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  // Maximum length of text string
void Print(const char* format , ...)
{
  char    buf[LEN];
  char*   ch=buf;
  va_list args;
  //  Turn the parameters into a character string
  va_start(args,format);
  vsnprintf(buf,LEN,format,args);
  va_end(args);
  //  Display the characters one at a time at the current raster position
  while (*ch)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

// calculates a set of points to be plotted
// using the lorenz attractor equation given
// in the lorenz.c example
int calculatePoints()
{
   /*  Time step  */
   double dt = 0.001;
   double scale = 0.025;

   double x = points_[0][0] = 1; // x
   double y = points_[0][1] = 1; // y
   double z = points_[0][2] = 1; // z
   /*
    *  Integrate 50,000 steps (50 time units with dt = 0.001)
    *  Explicit Euler integration
    */
   for (int i=1; i < num_points_; i++)
   {
      double dx = s_* (y-x);
      double dy = x * (r_-z) - y;
      double dz = x * y - b_ * z;
      x += dt*dx;
      y += dt*dy;
      z += dt*dz;

      points_[i][0] = scale * x;
      points_[i][1] = scale * y;
      points_[i][2] = scale * z;
   }
   return 0;
}

static void display()
{
  // calculate the set of points to be plotted
  calculatePoints();

  //  Clear the image
  glClear(GL_COLOR_BUFFER_BIT);
  //  Reset previous transforms
  glLoadIdentity();
  //  Set view angle
  glRotated(ph_,1,0,0);
  glRotated(th_,0,1,0);
  
  // draw axes
  glColor3f(1,1,1);
  glBegin(GL_LINES);
  glVertex3d(0,0,0);
  glVertex3d(1,0,0);
  glVertex3d(0,0,0);
  glVertex3d(0,1,0);
  glVertex3d(0,0,0);
  glVertex3d(0,0,1);
  glEnd();
  
  // Display lorenz points
  glBegin(GL_LINE_STRIP);

  for (int i = 1; i < num_points_; i++)
  {
    double color = (double)i / (double)num_points_;
    glColor3d(color,0.0,0.5/color);
    glVertex3dv(points_[i]);
  }

  glEnd();

  glColor3d(1.0,1.0,1.0);

    //  Label axes
  glRasterPos3d(1,0,0);
  Print("X");
  glRasterPos3d(0,1,0);
  Print("Y");
  glRasterPos3d(0,0,1);
  Print("Z");

  //  Display parameters
  glWindowPos2i(5,5);
  Print("View Angle=%d,%d  s=%f  b=%f  r=%f", 
    th_, ph_, s_, b_, r_);
  
  // flush and swap
  glFlush();
  glutSwapBuffers();
}

static void reshape(int width, int height)
{
  //  Ratio of the width to the height of the window
  double w2h = (height>0) ? (double)width/height : 1;
  //  Set the viewport to the entire window
  glViewport(0,0, width,height);
  //  Tell OpenGL we want to manipulate the projection matrix
  glMatrixMode(GL_PROJECTION);
  //  Undo previous transformations
  glLoadIdentity();
  //  Orthogonal projection box adjusted for the
  //  aspect ratio of the window
  glOrtho(-dim_*w2h,+dim_*w2h, -dim_,+dim_, -dim_,+dim_);
  //  Switch to manipulating the model matrix
  glMatrixMode(GL_MODELVIEW);
  //  Undo previous transformations
  glLoadIdentity();
}

static void special(int k, int x, int y)
{
  //  Right arrow key - increase azimuth by 5 degrees
  if (k == GLUT_KEY_RIGHT)
    th_ += 5;
  //  Left arrow key - decrease azimuth by 5 degrees
  else if (k == GLUT_KEY_LEFT)
    th_ -= 5;
  //  Up arrow key - increase elevation by 5 degrees
  else if (k == GLUT_KEY_UP)
    ph_ += 5;
  //  Down arrow key - decrease elevation by 5 degrees
  else if (k == GLUT_KEY_DOWN)
    ph_ -= 5;
  //  Keep angles to +/-360 degrees
  th_ %= 360;
  ph_ %= 360;
  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

static void key(unsigned char k, int x, int y)
{
  if (k == 'y')
    r_ += 0.5;
  else if (k == 'p')
    r_ -= 0.5;
  else if (k == 'i')
    b_ += 0.05;
  else if (k == 'u')
    b_ -= 0.05;
  else if (k == 'x')
    s_ += 0.5;
  else if (k == 'k')
    s_ -= 0.5;
  glutPostRedisplay();
}

static void idle()
{

}

int main(int argc, char *argv[])
{
  glutInit(&argc,argv);

  // set initial glut parameters
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowPosition(100,100);
  glutInitWindowSize(500,500);

  // create the window
  glutCreateWindow("hw2 - Andrew Kramer");

  // set callback functions
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(special);
  glutKeyboardFunc(key);
  glutIdleFunc(idle);

  glutMainLoop();

  return 0;
}