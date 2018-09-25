/*
 *  Scene in 3D
 *  
 *  Creates a watertight airplane in 3D
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
double zh=0;      //  Rotation of teapot
int axes=1;       //  Display axes
int mode=0;       //  What to display

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
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

double interp(double x1, double x2, double t)
{
  return t * (x1 + x2);
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
   //  Switch display mode
   else if (ch == 'm')
      mode = (mode+1)%7;
   else if (ch == 'M')
      mode = (mode+6)%7;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   const double dim=2.5;
   //  Ratio of the width to the height of the window
   double w2h = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection
   glOrtho(-w2h*dim,+w2h*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 * Draws a piper cub's fuselage including nose cowling and engine 
 */
static void drawFuselage()
{
	glBegin(GL_QUADS);

	// aft tail boom  right side
  glColor3f(1,0,0);
	glVertex3d(-0.75, 0.15, 0.0);
  glVertex3d(-0.75, 0.1, 0.0);
  glVertex3d(-0.15, 0.025, 0.0875);
  glVertex3d(-0.15, 0.17, 0.0875);

  // aft tail boom left side
  glColor3f(0,1,0);
  glVertex3d(-0.75, 0.1, 0.0);
  glVertex3d(-0.75, 0.15, 0.0);
  glVertex3d(-0.15, 0.17, -0.0875);
  glVertex3d(-0.15, 0.025, -0.0875);

  // aft tail boom top
  glColor3f(0,0,1);
  glVertex3d(-0.75, 0.15, 0.0);
  glVertex3d(-0.75, 0.15, 0.0);
  glVertex3d(-0.15, 0.17, 0.0875);
  glVertex3d(-0.15, 0.17, -0.0875);

  // aft tail boom bottom
  glColor3d(0.5,0.5,0);
  glVertex3d(-0.75, 0.1, 0.0);
  glVertex3d(-0.75, 0.1, 0.0);
  glVertex3d(-0.15, 0.025, -0.0875);
  glVertex3d(-0.15, 0.025, 0.0875);

  // fwd tail boom right side
  glColor3f(0.5,1,0);
  glVertex3d(-0.15, 0.17, 0.0875);
  glVertex3d(-0.15, 0.025, 0.0875);
  glVertex3d(0.0, 0.0, 0.10);
  glVertex3d(0.0, 0.20, 0.10);

  // fwd tail boom left side
  glColor3f(0,0.5,1);
  glVertex3d(-0.15, 0.025, -0.0875);
  glVertex3d(-0.15, 0.17, -0.0875);
  glVertex3d(0.0, 0.20, -0.10);
  glVertex3d(0.0, 0.0, -0.10);

  // fwd tail boom top
  glColor3f(0,1,1);
  glVertex3d(-0.15, 0.17, 0.0875);
  glVertex3d(0.0, 0.20, 0.10);
  glVertex3d(0.0, 0.20, -0.10);
  glVertex3d(-0.15, 0.17, -0.0875);

  // fwd tail boom bottom
  glColor3f(0,0.5,0.5);
  glVertex3d(-0.15, 0.025, -0.0875);
  glVertex3d(-0.15, 0.025, 0.0875);
  glVertex3d(0.0, 0.0, 0.10);
  glVertex3d(0.0, 0.0, -0.10);

  // right door
  glColor3f(0.5,0.5,0.5);
  glVertex3d(0.0, 0.20, 0.10);
  glVertex3d(0.0, 0.0, 0.10);
  glVertex3d(0.25, 0.0, 0.10);
  glVertex3d(0.25, 0.20, 0.10);

  // left door
  glColor3f(0.5,0,0.5);
  glVertex3d(0.0, 0.0, -0.10);
  glVertex3d(0.0, 0.20, -0.10);
  glVertex3d(0.25, 0.20, -0.10);
  glVertex3d(0.25, 0.0, -0.10);

  // belly
  glColor3f(0.5,0.25,0.25);
  glVertex3d(0.0, 0.0, 0.1);
  glVertex3d(0.0, 0.0, -0.1);
  glVertex3d(0.25, 0.0, -0.1);
  glVertex3d(0.25, 0.0, 0.1);

  // windscreen
  glColor3f(0.1,0.5,0.1);
  glVertex3d(0.25, 0.2, 0.1);
  glVertex3d(0.25, 0.2, -0.1);
  glVertex3d(0.33, 0.13, -0.09);
  glVertex3d(0.33, 0.13, 0.09);

  // fwd fuselage right
  glColor3f(0.25,0.25,0.1);
  glVertex3d(0.25, 0.2, 0.1);
  glVertex3d(0.25, 0.0, 0.1);
  glVertex3d(0.33, 0.01, 0.09);
  glVertex3d(0.33, 0.13, 0.09);

  // fwd fuselage left
  glColor3f(0.5, 0.1, 0.1);
  glVertex3d(0.25, 0.0, -0.1);
  glVertex3d(0.25, 0.2, -0.1);
  glVertex3d(0.33, 0.13, -0.09);
  glVertex3d(0.33, 0.01, -0.09);

  // fwd belly

  // aft cowling

  // fwd cowling

	glEnd();

}

static void drawPiperCub(double x,double y,double z,
       	           			double dx,double dy,double dz,
                  			double ux,double uy, double uz)
{
	//  Unit vector in direction of flght
   double D0 = sqrt(dx*dx+dy*dy+dz*dz);
   double X0 = dx/D0;
   double Y0 = dy/D0;
   double Z0 = dz/D0;
   //  Unit vector in "up" direction
   double D1 = sqrt(ux*ux+uy*uy+uz*uz);
   double X1 = ux/D1;
   double Y1 = uy/D1;
   double Z1 = uz/D1;
   //  Cross product gives the third vector
   double X2 = Y0*Z1-Y1*Z0;
   double Y2 = Z0*X1-Z1*X0;
   double Z2 = X0*Y1-X1*Y0;
   //  Rotation matrix
   double mat[16];
   mat[0] = X0;   mat[4] = X1;   mat[ 8] = X2;   mat[12] = 0;
   mat[1] = Y0;   mat[5] = Y1;   mat[ 9] = Y2;   mat[13] = 0;
   mat[2] = Z0;   mat[6] = Z1;   mat[10] = Z2;   mat[14] = 0;
   mat[3] =  0;   mat[7] =  0;   mat[11] =  0;   mat[15] = 1;

   // save current transforms
   glPushMatrix();

   // offset, scale and rotate
   glTranslated(x,y,z);
   glMultMatrixd(mat);

   drawFuselage();

   glPopMatrix();
}

void display()
{
	const double len=1.5;  //  Length of axes
  	//  Erase the window and the depth buffer
  	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  	//  Enable Z-buffering in OpenGL
  	glEnable(GL_DEPTH_TEST);
  	//  Undo previous transformations
  	glLoadIdentity();
  	//  Set view angle
  	glRotatef(ph,1,0,0);
  	glRotatef(th,0,1,0);

  	drawPiperCub(0,0,0, 1,0,0, 0,1,0);

  	glFlush();
  	glutSwapBuffers();
}

void idle()
{
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Andrew Kramer: hw3");
   //  Tell GLUT to call "idle" when there is nothing else to do
   glutIdleFunc(idle);
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}