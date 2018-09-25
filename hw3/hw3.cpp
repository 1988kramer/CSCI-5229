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
double zh_=0;      //  Rotation of teapot
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

void pointOnCircle(double th, double r, double c_x, double c_y, double c_z)
{
  glVertex3d(c_x, c_y + (r*Cos(th)), c_z + (r*Sin(th)));
}

/*
 * Draws a piper cub's fuselage including nose cowling and engine 
 */
static void drawFuselage()
{
	glBegin(GL_QUADS);

	// aft tail boom  right side
  double tail_top = 0.17;
  double fwd_tail_top = 0.21;
  double tail = -0.85;
  double tail_boom_front = -0.25;

  glColor3f(1,0,0);
	glVertex3d(tail, tail_top, 0.0);
  glVertex3d(tail, 0.1, 0.0);
  glVertex3d(tail_boom_front, 0.025, 0.0875);
  glVertex3d(tail_boom_front, fwd_tail_top, 0.0875);

  // aft tail boom left side
  glColor3f(0,1,0);
  glVertex3d(tail, 0.1, 0.0);
  glVertex3d(tail, tail_top, 0.0);
  glVertex3d(tail_boom_front, fwd_tail_top, -0.0875);
  glVertex3d(tail_boom_front, 0.025, -0.0875);

  // aft tail boom top
  glColor3f(0,0,1);
  glVertex3d(tail, tail_top, 0.0);
  glVertex3d(tail, tail_top, 0.0);
  glVertex3d(tail_boom_front, fwd_tail_top, 0.0875);
  glVertex3d(tail_boom_front, fwd_tail_top, -0.0875);

  // aft tail boom bottom
  glColor3d(0.5,0.5,0);
  glVertex3d(tail, 0.1, 0.0);
  glVertex3d(tail, 0.1, 0.0);
  glVertex3d(tail_boom_front, 0.025, -0.0875);
  glVertex3d(tail_boom_front, 0.025, 0.0875);

  double door_top = 0.25;
  double door_bottom = 0.0;
  double fwd_tail_front = -0.1;

  // fwd tail boom right side
  glColor3f(0.5,1,0);
  glVertex3d(tail_boom_front, fwd_tail_top, 0.0875);
  glVertex3d(tail_boom_front, 0.025, 0.0875);
  glVertex3d(fwd_tail_front, door_bottom, 0.10);
  glVertex3d(fwd_tail_front, door_top, 0.10);

  // fwd tail boom left side
  glColor3f(0,0.5,1);
  glVertex3d(tail_boom_front, 0.025, -0.0875);
  glVertex3d(tail_boom_front, fwd_tail_top, -0.0875);
  glVertex3d(fwd_tail_front, door_top, -0.10);
  glVertex3d(fwd_tail_front, door_bottom, -0.10);

  // fwd tail boom top
  glColor3f(0,1,1);
  glVertex3d(tail_boom_front, fwd_tail_top, 0.0875);
  glVertex3d(fwd_tail_front, door_top, 0.10);
  glVertex3d(fwd_tail_front, door_top, -0.10);
  glVertex3d(tail_boom_front, fwd_tail_top, -0.0875);

  // fwd tail boom bottom
  glColor3f(0,0.5,0.5);
  glVertex3d(tail_boom_front, 0.025, -0.0875);
  glVertex3d(tail_boom_front, 0.025, 0.0875);
  glVertex3d(fwd_tail_front, door_bottom, 0.10);
  glVertex3d(fwd_tail_front, door_bottom, -0.10);

  // right door
  glColor3f(0.5,0.5,0.5);
  glVertex3d(fwd_tail_front, door_top, 0.10);
  glVertex3d(fwd_tail_front, door_bottom, 0.10);
  glVertex3d(0.25, door_bottom, 0.10);
  glVertex3d(0.25, door_top, 0.10);

  // left door
  glColor3f(0.5,0,0.5);
  glVertex3d(fwd_tail_front, door_bottom, -0.10);
  glVertex3d(fwd_tail_front, door_top, -0.10);
  glVertex3d(0.25, door_top, -0.10);
  glVertex3d(0.25, door_bottom, -0.10);

  // belly
  glColor3f(0.5,0.25,0.25);
  glVertex3d(fwd_tail_front, door_bottom, 0.1);
  glVertex3d(fwd_tail_front, door_bottom, -0.1);
  glVertex3d(0.25, door_bottom, -0.1);
  glVertex3d(0.25, door_bottom, 0.1);

  double cowling_top = 0.18;
  double cowling_bottom = 0.01;
  double cowling_side = 0.09;
  double firewall = 0.33;

  // windscreen
  glColor3f(0.1,0.5,0.1);
  glVertex3d(0.25, door_top, 0.1);
  glVertex3d(0.25, door_top, -0.1);
  glVertex3d(firewall, cowling_top, -1. * cowling_side);
  glVertex3d(firewall, cowling_top, cowling_side);

  // fwd fuselage right
  glColor3f(0.25,0.25,0.1);
  glVertex3d(0.25, door_top, 0.1);
  glVertex3d(0.25, door_bottom, 0.1);
  glVertex3d(firewall, cowling_bottom, cowling_side);
  glVertex3d(firewall, cowling_top, cowling_side);

  // fwd fuselage left
  glColor3f(0.5, 0.1, 0.1);
  glVertex3d(0.25, door_bottom, -0.1);
  glVertex3d(0.25, door_top, -0.1);
  glVertex3d(firewall, cowling_top, -1. * cowling_side);
  glVertex3d(firewall, cowling_bottom, -1. * cowling_side);

  // fwd belly
  glColor3f(0.2, 0.1, 0.2);
  glVertex3d(0.25, door_bottom, -0.1);
  glVertex3d(firewall, cowling_bottom, -1. * cowling_side);
  glVertex3d(firewall, cowling_bottom, cowling_side);
  glVertex3d(0.25, door_bottom, 0.1);

  glEnd();

  // aft cowling
  double cowl_y_center = 0.5 * (cowling_top + cowling_bottom);
  glColor3f(0.1,0.6,0.2);
  glBegin(GL_QUAD_STRIP);
  double radius = 0.06;
  double fwd_cowl = 0.45;
  double horiz_increment = cowling_side * 0.5;
  double horiz_position = -1. * cowling_side;
  for (double th = -45; th <= 45; th += 22.5)
  {
    glVertex3d(firewall, cowling_top, horiz_position);
    pointOnCircle(th,radius,fwd_cowl,cowl_y_center,0.0);
    horiz_position += horiz_increment;
  }
  double vert_increment = 0.25*(cowling_top - cowling_bottom);
  double vert_position = cowling_top;
  for (double th = 45; th <= 135; th += 22.5)
  {
    glVertex3d(firewall, vert_position, cowling_side);
    pointOnCircle(th,radius,fwd_cowl,cowl_y_center,0.0);
    vert_position -= vert_increment;
  }
  horiz_position -= horiz_increment;
  for(double th = 135; th <= 225; th += 22.5)
  {
    glVertex3d(firewall, cowling_bottom, horiz_position);
    pointOnCircle(th,radius,fwd_cowl,cowl_y_center,0.0);
    horiz_position -= horiz_increment;
  }
  vert_position += vert_increment;
  for(double th = 225; th <= 315; th += 22.5)
  {
    glVertex3d(firewall, vert_position, -1 * cowling_side);
    pointOnCircle(th,radius,fwd_cowl,cowl_y_center,0.0);
    vert_position += vert_increment;
  }

  glEnd();

  // fwd cowling
  double nose = 0.49;
  glColor3f(0.7,0.1,0.3);
  glBegin(GL_TRIANGLE_FAN);
  glVertex3d(nose, cowl_y_center, 0.0);
  for (double th = 0; th <= 360; th += 22.5)
  {
    pointOnCircle(th,radius,fwd_cowl,cowl_y_center,0.0);
  }

	glEnd();

}

static void drawWing()
{
  // define wing cross section
  int num_points = 6;
  double cross_sec_x[] = {0.25, 0.26, 0.25, 0.23, 0.20, -0.1};
  double cross_sec_y[] = {0.25, 0.26, 0.27, 0.28, 0.29, 0.25};

  double wingtip = 1.2;

  glColor3f(0.3,0.1,0.4);
  for (int j = 0; j < 2; j++)
  {
    wingtip *= -1.0;

    glBegin(GL_POLYGON);
    for (int i = 0; i < num_points; i++)
    {
      glVertex3d(cross_sec_x[i], cross_sec_y[i], wingtip);
    }
    glEnd();
  }

  glColor3f(0.8,0.1,0.1);
  glBegin(GL_QUAD_STRIP);
  for (int i = 0; i < num_points; i++)
  {
    glVertex3d(cross_sec_x[i], cross_sec_y[i], wingtip);
    glVertex3d(cross_sec_x[i], cross_sec_y[i], -1.0*wingtip);
  }
  glVertex3d(cross_sec_x[0], cross_sec_y[0], wingtip);
  glVertex3d(cross_sec_x[0], cross_sec_y[0], -1.0*wingtip);
  glEnd();
}

static void drawVStab()
{
  int num_points = 7;
  double cross_sec_x[] = {-0.65, -0.85, -0.85, -0.95, -0.90, -0.85, -0.80};
  double cross_sec_y[] = {0.17, 0.17, 0.11, 0.14, 0.35, 0.38, 0.35};

  glColor3f(0.1,0.8,0.3);
  glBegin(GL_POLYGON);
  for (int i = 0; i < num_points; i++)
  {
    glVertex3d(cross_sec_x[i], cross_sec_y[i], 0.0);
  }
  glEnd();
}

static void drawHStab()
{
  double stab_height = 0.15;
  int num_points = 7;
  double cross_sec_x[] = {-0.65, -0.66, -0.71, -0.88, -0.95, -0.95, -0.87};
  double cross_sec_z[] = {0.0, 0.07, 0.15, 0.30, 0.25, 0.08, 0.0};

  glColor3f(0.7,0.3,0.5);
  
  for (int j = 0; j < 2; j++)
  {
    glBegin(GL_POLYGON);
    for (int i = 0; i < num_points; i++)
    {
      cross_sec_z[i] *= -1.0;
      glVertex3d(cross_sec_x[i], stab_height, cross_sec_z[i]);
    }
    glEnd();
  }
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
   drawWing();
   drawVStab();
   drawHStab();

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
  double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh_ = fmod(90*t,360);
   glutPostRedisplay();
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