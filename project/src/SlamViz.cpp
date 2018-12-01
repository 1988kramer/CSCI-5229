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
   Ylight = 3.0*dim;
   x = y = z = 0;
   l_mouse = r_mouse = false;         //  Mouse movement
   smooth    =   1;  // Smooth/Flat shading
   ambient   =  30;  // Ambient intensity (%)
   diffuse   = 100;  // Diffuse intensity (%)
   specular  =   0;  // Specular intensity (%)
   distance  =   5;  // Light distance
   zh        =  90;  // Light azimuth
   fov = 55;
   local     =   0;  // Local Viewer Model
   emission  =   0;  // Emission intensity (%)
   shiny   =   1;  // Shininess (value)
   cur_time = 0.0;
   last_time = 0.0;
   last_stamp = 0.0;
   scale_factor = 2.0;
   framebuf = 0;
   plane = new airplane(texture,3);
   light = pose_track = disp_inactive_lmrks = disp_prev_poses = disp_sky = axes = false; 
   lmrk_lwr_bound = 0.03;
   mode = true;
   timer = new QTimer(this);
   connect(timer, SIGNAL(timeout()), this, SLOT(timerEvent()));
   timer->start(16);
   pose_file = new std::ifstream();
   pose_file->open("pose_log.txt");
   lmrk_file = new std::ifstream();
   lmrk_file->open("lmrk_log.txt");
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

void SlamViz::setLmrkDispBound(double bound)
{
   lmrk_lwr_bound = bound;
   update();
}

void SlamViz::toggleInactive(void)
{
   disp_inactive_lmrks = !disp_inactive_lmrks;
   update();
}

void SlamViz::togglePoseTrack(void)
{
   pose_track = !pose_track;
   update();
}

void SlamViz::togglePrevPoses(void)
{
   disp_prev_poses = !disp_prev_poses;
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
   emit dimen(QString::number(dim));
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
   Ylight = 3.0*dim;
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
   sky = new QOpenGLTexture(QImage(QString("sky2.jpg")));

   initShaders();

   initMap();
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
      readLmrks();
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
      //gluLookAt(Ex+x,Ey+y,Ez+z, x,y,z, 0,Cos(ph),0);
      gluLookAt(Ex,Ey,Ez, 0,0,0, 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
      
   }
   // track pose if pose tracking enabled
   glTranslated(-x,-y,-z);

   

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Translate intensity to color vectors
   float Ambient[]   = {float(0.01*ambient),float(0.01*ambient),float(0.01*ambient),1.0};
   float Diffuse[]   = {float(0.01*diffuse),float(0.01*diffuse),float(0.01*diffuse),1.0};
   float Specular[]  = {float(0.01*specular),float(0.01*specular),float(0.01*specular),1.0};
   
   //  switch light from at sun position to orbiting
   float Position[4];
   Position[0] = float(3.0*dim);
   Position[1] = Ylight;
   Position[2] = 0.0; 
   Position[3] = 1.0;

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
   //if (!mode && pose_track)
   //   glTranslated(-x,-y,-z);
   Scene(1);

   //  Draw axes - no lighting from here on
   

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
   int inc = 30;
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
   sky->bind();
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
         translation[i] = scale_factor*std::stof(token);
      }

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
      if (pose_track)
      {
         x = translation[0];
         y = translation[2];
         z = -translation[1];
      }
      else
      {
         x = y = z = 0;
      }
   }
}

void SlamViz::readLmrks()
{
   std::string line;
   if (std::getline(*lmrk_file, line))
   {
      // insert new landmarks
      unsigned int stamp = std::stod(line);
      std::getline(*lmrk_file, line);
      while (line != "")
      {
         std::istringstream ss(line);
         std::string token;
         std::getline(ss, token, ' ');
         unsigned long id = std::stol(token);
         Landmark lmrk;
         lmrk.timestamp = stamp;
         std::getline(ss, token, ' ');
         lmrk.quality = std::stod(token);
         for (int i = 0; i < 3; i++)
         {
            std::getline(ss, token, ' ');
            lmrk.point[i] = scale_factor*std::stof(token);
         }
         // update timestamp if landmark already exists
         if (lmrks.find(id) != lmrks.end())
         {
            lmrks.at(id) = lmrk;
         }
         else
         {
            lmrks.insert(std::pair<unsigned long, Landmark>(id, lmrk));
         }
         std::getline(*lmrk_file, line);
      }
      // remove old landmarks
      std::vector<unsigned long> marginalized_ids;
      for (std::map<unsigned long, Landmark>::iterator it = lmrks.begin();
         it != lmrks.end(); it++)
      {
         if (it->second.timestamp < stamp)
            marginalized_ids.push_back(it->first);
      }
      for (int i = 0; i < marginalized_ids.size(); i++)
      {
         Landmark marginalized = lmrks.at(marginalized_ids[i]);
         lmrks.erase(marginalized_ids[i]);
         inactive_lmrks.insert(std::pair<unsigned long, Landmark>(marginalized_ids[i],marginalized));
      }
   }
}

void SlamViz::drawAxes(double len, bool draw_labels)
{
   glDisable(GL_LIGHTING);
   glBegin(GL_LINES);
   glColor3d(1.0,0.0,0.0);
   glVertex3d(0.0,0.0,0.0);
   glVertex3d(len,0.0,0.0);
   glColor3d(0.0,1.0,0.0);
   glVertex3d(0.0,0.0,0.0);
   glVertex3d(0.0,len,0.0);
   glColor3d(0.0,0.0,1.0);
   glVertex3d(0.0,0.0,0.0);
   glVertex3d(0.0,0.0,len);
   glEnd();
   glColor3d(1.0,1.0,1.0);
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

void SlamViz::initShaders()
{
   // compile shader for shadowing
   if (!shadow_shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shadow.vert"));
      close();
   if (!shadow_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shadow.frag"));
      close();
   if (!shadow_shader.link())
      close();
   if (!shadow_shader.bind())
      close();
}

void SlamViz::initMap()
{
   unsigned int shadowtex;
   int n;

   // make sure multitextures are supported
   glGetIntegerv(GL_MAX_TEXTURE_UNITS, &n);
   if (n<2) close();

   // get max texture buffer size
   glGetIntegerv(GL_MAX_TEXTURE_SIZE, &shadowdim);
   // limit texture size to maximum buffer size
   glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE,&n);
   if (shadowdim > n) shadowdim = n;
   // limit texture size to 2048 for performance
   if (shadowdim > 2048) shadowdim = 2048;
   if (shadowdim < 512) close(); // shadow dimension too small

   // do shadow textures in multitexture 1
   glActiveTexture(GL_TEXTURE1);
   glGenTextures(1,&shadowtex);
   glBindTexture(GL_TEXTURE_2D, shadowtex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowdim, shadowdim, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

   glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

   //  Set texture mapping to clamp and linear interpolation
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

   //  Set automatic texture generation mode to Eye Linear
   glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
   glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
   glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
   glTexGeni(GL_Q,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);

   // Switch back to default textures
   glActiveTexture(GL_TEXTURE0);

   // Attach shadow texture to frame buffer
   glGenFramebuffers(1,&framebuf);
   glBindFramebuffer(GL_FRAMEBUFFER,framebuf);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowtex, 0);
   //  Don't write or read to visible color buffer
   glDrawBuffer(GL_NONE);
   glReadBuffer(GL_NONE);
   //  Make sure this all worked
   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) Fatal("Error setting up frame buffer\n");
   glBindFramebuffer(GL_FRAMEBUFFER,0);

   shadowMap();
}

void SlamViz::shadowMap(void)
{
   double Lmodel[16];
   double Lproj[16];
   double Tproj[16];
   double Dim = 2.0;
   double Ldist;

   glPushMatrix();
   glPushAttrib(GL_TRANSFORM_BIT|GL_ENABLE_BIT);
   glShadeModel(GL_FLAT);
   glColorMask(0,0,0,0);
   glEnable(GL_POLYGON_OFFSET_FILL);

   Light(0);

   Ldist = sqrt(Lpos[0]*Lpos[0] + Lpos[1]*Lpos[1] + Lpos[2]*Lpos[2]);
   if(Ldist < 1.1*dim) Ldist = 1.1*dim;

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(114.6*atan(Dim/Ldist),1,Ldist-Dim,Ldist+Dim);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(Lpos[0],Lpos[1],Lpos[2], 0,0,0, 0,1,0);
   glViewport(0,0,shadowdim,shadowdim);

   glBindFramebuffer(GL_FRAMEBUFFER, framebuf);
   glClear(GL_DEPTH_BUFFER_BIT);

   Scene(0);

   glGetDoublev(GL_PROJECTION_MATRIX,Lproj);
   glGetDoublev(GL_MODELVIEW_MATRIX,Lmodel);

   // Set up texture matrix for shadow map projection,
   // which will be rolled into the eye linear
   // texture coordinate generation plane equations
   glLoadIdentity();
   glTranslated(0.5,0.5,0.5);
   glScaled(0.5,0.5,0.5);
   glMultMatrixd(Lproj);
   glMultMatrixd(Lmodel);

   // Retrieve result and transpose to get the s, t, r, and q rows for plane equations
   glGetDoublev(GL_MODELVIEW_MATRIX,Tproj);
   Svec[0] = Tproj[0];    Tvec[0] = Tproj[1];    Rvec[0] = Tproj[2];    Qvec[0] = Tproj[3];
   Svec[1] = Tproj[4];    Tvec[1] = Tproj[5];    Rvec[1] = Tproj[6];    Qvec[1] = Tproj[7];
   Svec[2] = Tproj[8];    Tvec[2] = Tproj[9];    Rvec[2] = Tproj[10];   Qvec[2] = Tproj[11];
   Svec[3] = Tproj[12];   Tvec[3] = Tproj[13];   Rvec[3] = Tproj[14];   Qvec[3] = Tproj[15];

   // Restore normal drawing state
   glShadeModel(GL_SMOOTH);
   glColorMask(1,1,1,1);
   glDisable(GL_POLYGON_OFFSET_FILL);
   glPopAttrib();
   glPopMatrix();
   glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void SlamViz::Light(int light)
{
   //  Set light position
   Lpos[0] = 2*Cos(zh);
   Lpos[1] = Ylight;
   Lpos[2] = 2*Sin(zh);
   Lpos[3] = 1;

   //  Enable lighting
   if (light)
   {
      float Med[]  = {0.3,0.3,0.3,1.0};
      float High[] = {1.0,1.0,1.0,1.0};
      //  Enable lighting with normalization
      glEnable(GL_LIGHTING);
      glEnable(GL_NORMALIZE);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0,GL_POSITION,Lpos);
      glLightfv(GL_LIGHT0,GL_AMBIENT,Med);
      glLightfv(GL_LIGHT0,GL_DIFFUSE,High);
   }
   else
   {
      glDisable(GL_LIGHTING);
      glDisable(GL_COLOR_MATERIAL);
      glDisable(GL_NORMALIZE);
   }
}

void SlamViz::Scene(int light)
{
   Light(light);

   glPushMatrix();
   //  Draw scene
   glRotated(-90.0,1.0,0.0,0.0);
   glMultMatrixf(glm::value_ptr(cur_pose.T_WS));
   plane->drawAirplane(0,0,0,
                       0,0,1,
                       1,0,0);
   glPopMatrix();

   for (std::map<unsigned long, Landmark>::iterator it = lmrks.begin();
      it != lmrks.end(); it++)
   {
      if (it->second.quality >= lmrk_lwr_bound)
      {
         glPushMatrix();
         glRotated(-90.0,1.0,0.0,0.0);
         double x = it->second.point[0];
         double y = it->second.point[1];
         double z = it->second.point[2];
         ball(x,y,z,scale_factor*it->second.quality);
         glPopMatrix();
      }
   }

   if (disp_inactive_lmrks)
   {
      for (std::map<unsigned long, Landmark>::iterator it = inactive_lmrks.begin();
         it != inactive_lmrks.end(); it++)
      {
         if (it->second.quality >= lmrk_lwr_bound)
         {
            glPushMatrix();
            glRotated(-90.0,1.0,0.0,0.0);
            double x = it->second.point[0];
            double y = it->second.point[1];
            double z = it->second.point[2];
            ball(x,y,z,0.05);
            glPopMatrix();
         }
      }
   }

   // prior poses, axes and skybox don't cast shadows, so return
   // here if not doing lighting
   if (!light) return;
   else glDisable(GL_TEXTURE_2D);

   if (axes)
      drawAxes(2.0, true);

   if (disp_sky)
      Sky(3.0*dim);
   else
      displayGrid(5);

   if (disp_prev_poses)
   {
      for (int i = 0; i < prev_poses.size(); i++)
      {
         glPushMatrix();
         glRotated(-90.0,1.0,0.0,0.0);
         glMultMatrixf(glm::value_ptr(prev_poses[i].T_WS));
         drawAxes(0.5,false);
         glPopMatrix();
      }
   }
}

