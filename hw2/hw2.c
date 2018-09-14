/*
 * Simple program to demonstrate generating coordinates
 * using the Lorenz Attractor
 */

#include <stdio.h>
#include <stdlib.h>
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
double theta_ = 0.0;
double points_[50000][3];

static void display()
{

}

static void reshape(int width, int height)
{

}

static void special(int k, int x, int y)
{

}

static void key(unsigned char key, int x, int y)
{

}

static void idle()
{

}

// calculates a set of points to be plotted
// using the lorenz attractor equation given
// in the lorenz.c example
int calculatePoints()
{
   /*  Time step  */
   double dt = 0.001;

   points_[0][0] = 1; // x
   points_[0][1] = 1; // y
   points_[0][2] = 1; // z
   /*
    *  Integrate 50,000 steps (50 time units with dt = 0.001)
    *  Explicit Euler integration
    */
   for (int i=1;i<50000;i++)
   {
      double dx = s_*(points_[i-1][1]-points_[i-1][0]);
      double dy = points_[i-1][0]*(r_-points_[i-1][2])-points_[i-1][1];
      double dz = points_[i-1][0]*points_[i-1][1] - b_*points_[i-1][2];
      points_[i][0] += dt*dx;
      points_[i][1] += dt*dy;
      points_[i][2] += dt*dz;
   }
   return 0;
}

int main(int argc, char *argv[])
{
  // calculate the set of points to be plotted
  calculatePoints();

  glutInit(&argc,argv);

  // set initial glut parameters
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowPosition(100,100);
  glutInitWindowSize(500,500);

  // create the window
  glutCreateWindow("Lorenz Attractor - Andrew Kramer");

  // set callback functions
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(special);
  glutKeyboardFunc(key);
  glutIdleFunc(idle);

  glutMainLoop();

  return 0;
}