//
//  OpenGL Lorenz Widget
//
#include <QtOpenGL>
#include "SlamViz.h"
#include "CSCIx229.h"

//
//  Constructor
//
SlamViz::SlamViz(QWidget* parent)
    : QOpenGLWidget(parent)
{
   th = ph = 30;      //  Set intial display angles
   asp = 1;           //  Aspect ratio
   dim = 50;          //  World dimension
   x0 = y0 = z0 = 1;  //  Starting location
   mouse = 0;         //  Mouse movement
   smooth    =   1;  // Smooth/Flat shading
   ambient   =  30;  // Ambient intensity (%)
   diffuse   = 100;  // Diffuse intensity (%)
   specular  =   0;  // Specular intensity (%)
   distance  =   5;  // Light distance
   zh        =  90;  // Light azimuth
   ylight = 0;
   local     =   0;  // Local Viewer Model
   emission  =   0;  // Emission intensity (%)
   shiny   =   1;  // Shininess (value)
   inc       =  10;  // Ball increment
   plane = new airplane(texture,3);
}

/********************************************************************/
/*************************  Set parameters  *************************/
/********************************************************************/

//
// toggle display of axes
//
void SlamViz::toggleAxes(void)
{
   axes = !axes;
   update();
}

//
// toggle lighting
//
void SlamViz::toggleLight(void)
{
   light = !light;
   update();
}

//
// toggle projection mode
//
void SlamViz::toggleDisplay(void)
{
   mode = !mode;
   update();
}

//
//  Reset view angle
//
void SlamViz::reset(void)
{
   th = ph = 0;  //  Set parameter
   update();     //  Request redisplay
}

//
//  Set dim
//
void SlamViz::setDIM(double DIM)
{
   dim = DIM;    //  Set parameter
   Project(55, dim, asp);
   update();     //  Request redisplay
}

/******************************************************************/
/*************************  Mouse Events  *************************/
/******************************************************************/
//
//  Mouse pressed
//
void SlamViz::mousePressEvent(QMouseEvent* e)
{
   mouse = true;
   pos = e->pos();  //  Remember mouse location
}

//
//  Mouse released
//
void SlamViz::mouseReleaseEvent(QMouseEvent*)
{
    mouse = false;
}

//
//  Mouse moved
//
void SlamViz::mouseMoveEvent(QMouseEvent* e)
{
   if (mouse)
   {
      QPoint d = e->pos()-pos;  //  Change in mouse location
      th = (th+d.x())%360;      //  Translate x movement to azimuth
      ph = (ph+d.y())%360;      //  Translate y movement to elevation
      pos = e->pos();           //  Remember new location
      update();                 //  Request redisplay
   }
}

//
//  Mouse wheel
//
void SlamViz::wheelEvent(QWheelEvent* e)
{
   //  Zoom out
   if (e->delta()<0)
      setDIM(dim+1);
   //  Zoom in
   else if (dim>2)
      setDIM(dim-1);
   //  Signal to change dimension spinbox
   emit dimen(dim);
}

/*******************************************************************/
/*************************  OpenGL Events  *************************/
/*******************************************************************/

//
//  Initialize
//
void SlamViz::initializeGL()
{
// glEnable(GL_DEPTH_TEST); //  Enable Z-buffer depth testing
   std::cout << "initializing" << std::endl;
   setMouseTracking(true);  //  Ask for mouse events
   initializeOpenGLFunctions();
   texture[0] = new QOpenGLTexture(QImage(QString("yellow_fabric.bmp")));
   texture[1] = new QOpenGLTexture(QImage(QString("metal.bmp")));
   texture[2] = new QOpenGLTexture(QImage(QString("bricks.bmp")));
   std::cout << "initialized" << std::endl;
}

//
//  Window is resized
//
void SlamViz::resizeGL(int width, int height)
{
   //  Window aspect ration
   asp = (width && height) ? width / (float)height : 1;
   //  Viewport is whole screen
   glViewport(0,0,width,height);
   //  Set projection
   Project(55, dim, asp);
}

//
//  Draw the window
//
void SlamViz::paintGL()
{
   const double len=2.0;
   //  Clear screen and Z-buffer
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   //  Reset transformations
   glLoadIdentity();
   //  Set rotation
   glRotated(ph , 1,0,0);
   glRotated(th , 0,1,0);

   // set projection
   if (mode)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Light switch
   if (light)
   {
      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      //  Light position
      float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
      //  Draw light position as ball (still no lighting here)
      glColor3f(1,1,1);
      ball(Position[0],Position[1],Position[2] , 0.1);
      //  OpenGL should normalize normal vectors
      glEnable(GL_NORMALIZE);
      //  Enable lighting
      glEnable(GL_LIGHTING);
      //  Location of viewer for specular calculations
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      //  Set ambient, diffuse, specular components and position of light 0
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
      glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
      glLightfv(GL_LIGHT0,GL_POSITION,Position);
   }
   else
      glDisable(GL_LIGHTING);

   //  Draw scene
   plane->drawAirplane(0,0,0,
                          1,0,0,
                          0,1,0);

   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //QFont f("Helvetica");
      //  Label axes
      //glRasterPos3d(len,0.0,0.0);
      //Print("X");
      //glRasterPos3d(0.0,len,0.0);
      //Print("Y");
      //glRasterPos3d(0.0,0.0,len);
      //Print("Z");
      //renderText(len, 0.0, 0.0, QString("X"));
      //renderText(0.0, len, 0.0, QString("Y"));
      //renderText(0.0, 0.0, len, QString("Z"));
   }

   //
   //  Emit signal with display angles and dimensions
   //
   emit angles("th,ph= "+QString::number(th)+","+QString::number(ph));

   //  Done
   glFlush();
}

/*
 *  Draw vertex in polar coordinates with normal
 */
void SlamViz::Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

void SlamViz::ball(double x,double y,double z,double r)
{
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,1);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}