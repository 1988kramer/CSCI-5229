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
    : QGLWidget(parent)
{
   th = ph = 30;      //  Set intial display angles
   asp = 1;           //  Aspect ratio
   dim = 20;          //  World dimension
   x0 = y0 = z0 = 1;  //  Starting location
   x = y = z = 0;
   l_mouse = r_mouse = false;         //  Mouse movement
   smooth    =   1;  // Smooth/Flat shading
   ambient   =  30;  // Ambient intensity (%)
   diffuse   = 100;  // Diffuse intensity (%)
   specular  =   0;  // Specular intensity (%)
   distance  =   5;  // Light distance
   zh        =  90;  // Light azimuth
   ylight = 0;
   fov = 55;
   local     =   0;  // Local Viewer Model
   emission  =   0;  // Emission intensity (%)
   shiny   =   1;  // Shininess (value)
   inc       =  10;  // Ball increment
   cur_time = 0.0;
   last_time = 0.0;
   last_stamp = 0.0;
   plane = new airplane(texture,3);
   disp_sky = axes = false; 
   light = mode = true;
   timer = new QTimer(this);
   connect(timer, SIGNAL(timeout()), this, SLOT(timerEvent()));
   timer->start(16);
   pose_file = new std::ifstream();
   pose_file->open("test_log.txt");
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
// toggle between sky and gridworld display
//
void SlamViz::toggleSky(void)
{
   disp_sky = !disp_sky;
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
   project();
   update();
}

void SlamViz::switchTexture(void)
{
   plane->changeTexture();
   update();
}

//
//  Reset view angle
//
void SlamViz::reset(void)
{
   th = ph = 0;  //  Set parameter
   x = y = z = 0;
   update();     //  Request redisplay
}

//
//  Set dim
//
void SlamViz::setDIM(double DIM)
{
   dim = DIM;    //  Set parameter
   project();
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
   if (e->button() == Qt::RightButton)
      r_mouse = true;
   if (e->button() == Qt::LeftButton)
      l_mouse = true;
   pos = e->pos();  //  Remember mouse location
}

//
//  Mouse released
//
void SlamViz::mouseReleaseEvent(QMouseEvent* e)
{
   if (e->button() == Qt::RightButton)
      r_mouse = false;
   else if (e->button() == Qt::LeftButton)
      l_mouse = false;
}

//
//  Mouse moved
//
void SlamViz::mouseMoveEvent(QMouseEvent* e)
{
   QPoint d = e->pos()-pos;  //  Change in mouse location
   // rotate field of view if right mouse
   if (r_mouse)
   {
      th = (th+d.x())%360;      //  Translate x movement to azimuth
      ph = (ph+d.y())%360;      //  Translate y movement to elevation
   }
   // translate field of view if left mouse
   if (l_mouse)
   {
      x += (d.x()*Cos(th)*Sin(ph) + d.y()*Sin(th)*Cos(ph))/ 10.0;
      y -= (d.y()*Sin(th) + d.x()*Sin(ph))/ 10.0;
   }
   pos = e->pos();           //  Remember new location
   update();                 //  Request redisplay
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
   else if (dim>1)
      setDIM(dim-1);
   //  Signal to change dimension spinbox
   //emit dimen("dimension: " + QString::number(dim));
}

/*******************************************************************/
/*************************  OpenGL Events  *************************/
/*******************************************************************/

//
//  Initialize
//
void SlamViz::initializeGL()
{
   glEnable(GL_DEPTH_TEST); //  Enable Z-buffer depth testing
   setMouseTracking(true);  //  Ask for mouse events
   texture[0] = new QOpenGLTexture(QImage(QString("yellow_fabric.bmp")));
   texture[1] = new QOpenGLTexture(QImage(QString("metal.bmp")));
   texture[2] = new QOpenGLTexture(QImage(QString("bricks.bmp")));
   sky[0] = new QOpenGLTexture(QImage(QString("sky0.bmp")));
   sky[1] = new QOpenGLTexture(QImage(QString("sky1.bmp")));
   sky[2] = new QOpenGLTexture(QImage(QString("sky2.jpg")));
}

void SlamViz::timerEvent(void)
{
   cur_time += 16;
   zh = (zh + 1) % 360;
   if (cur_time - last_time >= 64)
   {
      last_time = cur_time;
      addToPrevPoses();
      readPose();
   }
   update();
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
   project();
}

//
//  Draw the window
//
void SlamViz::paintGL()
{
   //  Clear screen and Z-buffer
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);

   //  Reset transformations
   glLoadIdentity();
   //  Set rotation
   //glRotated(ph , 1,0,0);
   //glRotated(th , 0,1,0);

   // set projection
   if (mode)
   {
      double Ex = (-2)*dim*Sin(th)*Cos(ph);
      double Ey = (2)*dim        *Sin(ph);
      double Ez = (2)*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez, 0,0,0, 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }
   if (disp_sky)
      Sky(3.0*dim);
   else
      displayGrid(10);

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Translate intensity to color vectors
   float Ambient[]   = {float(0.01*ambient),float(0.01*ambient),float(0.01*ambient),1.0};
   float Diffuse[]   = {float(0.01*diffuse),float(0.01*diffuse),float(0.01*diffuse),1.0};
   float Specular[]  = {float(0.01*specular),float(0.01*specular),float(0.01*specular),1.0};
   
   //  switch light from at sun position to orbiting
   float Position[4];
   if (light)
   {
      Position[0] = 1.5*float(distance*Cos(zh));
      Position[1] = 1.5*float(ylight);
      Position[2] = 1.5*float(distance*Sin(zh));
      Position[3] = 1.0;
   }
   else
   {
      Position[0] = float(3.0*dim);
      Position[1] = float(3.0*dim);
      Position[2] = 0.0; 
      Position[3] = 1.0;
   }

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

   glPushMatrix();
   //  Draw scene
   glRotated(-90.0,1.0,0.0,0.0);
   glMultMatrixf(glm::value_ptr(cur_pose.T_WS));
   plane->drawAirplane(0,0,0,
                       0,0,1,
                       1,0,0);
   glPopMatrix();

   for (int i = 0; i < prev_poses.size(); i++)
   {
      glPushMatrix();
      glRotated(-90.0,1.0,0.0,0.0);
      glMultMatrixf(glm::value_ptr(prev_poses[i].T_WS));
      drawAxes(0.5,false);
      glPopMatrix();
   }

   //  Draw axes - no lighting from here on
   if (axes)
      drawAxes(2.0, true);

   //
   //  Emit signal with display angles and dimensions
   //
   //emit angles("th,ph= "+QString::number(th)+","+QString::number(ph));

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
   float Emission[]  = {0.0,0.0,float(0.01*emission),1.0};
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

//
//  Set projection
//
void SlamViz::project()
{
   //  Orthogonal projection to dim
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   //if (asp>1)
   //   glOrtho(-dim*asp, +dim*asp, -dim, +dim, -8*dim, + 8*dim);
   if (mode)
      gluPerspective(fov,asp,dim/16,16*dim);
   else
      glOrtho(-dim, +dim, -dim/asp, +dim/asp, -8*dim, + 8*dim);

   //  Back to model view
   glMatrixMode(GL_MODELVIEW);
}

void SlamViz::displayGrid(double D)
{
   double limit = dim / 2.0;
   glBegin(GL_LINES);
   glColor3f(1.0,0.0,0.0);
   glLineWidth(1.0);
   glVertex3d(-D*limit,0.0,0.0);
   glVertex3d(D*limit,0.0,0.0);
   glVertex3d(0.0,0.0,-D*limit);
   glVertex3d(0.0,0.0,D*limit);

   glColor3f(1.0,1.0,1.0);
   glLineWidth(0.25);
   for (int i = -limit; i <= limit; i++)
   {
      if (i != 0)
      {
         glVertex3d(-D*limit,0.0,D*i);
         glVertex3d(D*limit,0.0,D*i);
      }
   }
   for (int i = -limit; i <= limit; i++)
   {
      if (i != 0)
      {
         glVertex3d(D*i,0.0,-D*limit);
         glVertex3d(D*i,0.0,D*limit);
      }
   }

   glEnd();
}

void SlamViz::Sky(double D)
{
   glColor3f(1,1,1);
   glEnable(GL_TEXTURE_2D);

   //  Sides
   sky[2]->bind();
   glBegin(GL_QUADS);
   glTexCoord2f(0.25,0.6667); glVertex3f(-D,-D,-D);
   glTexCoord2f(0.5,0.6667); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.5,0.3333); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.25,0.3333); glVertex3f(-D,+D,-D);

   glTexCoord2f(0.5,0.6667); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.75,0.6667); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.75,0.3333); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.5,0.3333); glVertex3f(+D,+D,-D);

   glTexCoord2f(0.75,0.6667); glVertex3f(+D,-D,+D);
   glTexCoord2f(1.0,0.6667); glVertex3f(-D,-D,+D);
   glTexCoord2f(1.0,0.3333); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.75,0.3333); glVertex3f(+D,+D,+D);

   glTexCoord2f(0.0,0.6667); glVertex3f(-D,-D,+D);
   glTexCoord2f(0.25,0.6667); glVertex3f(-D,-D,-D);
   glTexCoord2f(0.25,0.3333); glVertex3f(-D,+D,-D);
   glTexCoord2f(0.0,0.3333); glVertex3f(-D,+D,+D);
   glEnd();

   //  Top and bottom
   //sky[1]->bind();
   glBegin(GL_QUADS);
   glTexCoord2f(0.5,0.3334); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.5,0.0); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.25,0.0); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.25,0.3334); glVertex3f(-D,+D,-D);

   glTexCoord2f(0.25,1.0); glVertex3f(-D,-D,+D);
   glTexCoord2f(0.5,1.0); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.5,0.6667); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.25,0.6667); glVertex3f(-D,-D,-D);
   glEnd();

   glDisable(GL_TEXTURE_2D);
}

void SlamViz::readPose()
{
   std::string line;
   if (std::getline(*pose_file,line))
   {
      std::istringstream ss(line);
      std::string token;
      std::getline(ss, token, ' ');
      cur_pose.timestamp = std::stod(token);
      glm::vec3 translation;
      for (int i = 0; i < 3; i++)
      {
         std::getline(ss, token, ' ');
         translation[i] = 2.0*std::stof(token);
      }
      // swap y and z
      //double temp = translation[2];
      //translation[2] = translation[1];
      //translation[1] = temp;

      // swap x and y
      //temp = translation[2];
      //translation[2] = translation[0];
      //translation[0] = temp;

      std::vector<float> quat_vals;
      for (int i = 0; i < 4; i++)
      {
         std::getline(ss, token, ' ');
         quat_vals.push_back(std::stof(token));
      }
      glm::quat rotation(quat_vals[3],quat_vals[0],quat_vals[1],quat_vals[2]);
      glm::mat4 rotation_mat = glm::toMat4(rotation);
      glm::mat4 T_mat = glm::translate(glm::mat4(1), translation);

      cur_pose.T_WS = T_mat * rotation_mat;
   }
}

void SlamViz::drawAxes(double len, bool draw_labels)
{
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   glBegin(GL_LINES);
   glVertex3d(0.0,0.0,0.0);
   glVertex3d(len,0.0,0.0);
   glVertex3d(0.0,0.0,0.0);
   glVertex3d(0.0,len,0.0);
   glVertex3d(0.0,0.0,0.0);
   glVertex3d(0.0,0.0,len);
   glEnd();
   if (draw_labels)
   {
      renderText(len, 0.0, 0.0, QString("X"));
      renderText(0.0, len, 0.0, QString("Y"));
      renderText(0.0, 0.0, len, QString("Z"));
   }
}

// add pose to previous pose vector if it is above a 
// threshold distance to the last pose in that vector
void SlamViz::addToPrevPoses()
{
   if (prev_poses.size() == 0)
   {
      prev_poses.push_back(cur_pose);
   }
   else
   {
      // get translation component from current pose and
      // last pose in previous pose vector
      glm::vec3 scale;
      glm::quat rotation;
      glm::vec3 cur_trans;
      glm::vec3 prev_trans;
      glm::vec3 skew;
      glm::vec4 perspective;
      glm::decompose(cur_pose.T_WS, scale, rotation, cur_trans, skew, perspective);
      glm::decompose(prev_poses.back().T_WS, scale, rotation, prev_trans, skew, perspective);


      // calculate the magnitude of the translation
      // between the current pose and last pose
      double dist = sqrt(glm::length2(cur_trans - prev_trans));

      // if distance from last pose is greater than threshold, 
      // add to previous pose vector
      if (dist > 0.5)
         prev_poses.push_back(cur_pose);
   }
}