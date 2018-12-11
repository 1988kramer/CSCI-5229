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
  th = ph        =  30; //  Set intial display angles
  asp            =   1; //  Aspect ratio
  dim            =  20; //  World dimension
  v_x            =   0; //  x coordinate of view center
  v_y            =   0; //  y coordinate of view center
  v_z            =   0; //  z coordinate of view center
  fov            =  55; //  field of view degrees
  scale_factor   = 2.0; //  scale factor used for poses
  framebuf       =   0; //  frame buffer for shadowmap (not used in final)
  lmrk_lwr_bound = 0.03; // Lower bound for landmark quality
  timestep       =   0; //  current timestep being displayed
  frame_ms       =  50; //  milliseconds each frame is displayed

  r_mouse             = false;  //  Right mouse clicked
  pose_track          = false;  //  Camera view center tracks robot location
  disp_inactive_lmrks = false;  //  Display inactive (marginalized) landmarks
  disp_prev_poses     = false;  //  Display previous robot poses
  disp_sky            = false;  //  Display skybox if true, gridworld if false
  axes                = false;  //  Display center axes (and previous poses as
  															//    axes if disp_prev_poses is true)
  mode                = true;   //  Perspective projection if true, 
  															//    orthogonal if false
  run_fwd             = true;   //  Is visualization running forward
  paused              = false;  //  Is visualization paused
  
  // load poses and landmarks from log files
  if (! (readPoses() && readLmrks()))
  {
  	std::cerr << "could not open log files" << std::endl;
  	close();
  }

  // initialize timer function
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(timerEvent()));
  timer->start(frame_ms);
}

/********************************************************************/
/*************************  Set parameters  *************************/
/********************************************************************/


// toggle display of axes
void SlamViz::toggleAxes(void)
{
  axes = !axes;
  update();
}

// toggle between sky and gridworld display
void SlamViz::toggleSky(void)
{
  disp_sky = !disp_sky;
  update();
}

// set the lower quality bound for landmark display
void SlamViz::setLmrkDispBound(double bound)
{
  lmrk_lwr_bound = bound;
  update();
}

// set playback speed in frames per second
void SlamViz::setFPS(int fps)
{
	if (fps < 0)
		run_fwd = false;
	else
		run_fwd = true;

	frame_ms = std::abs(int((1.0/double(fps))*1000.0));
	if (std::abs(fps) > 0)
	{
		if (paused)
		{
			paused = false;
			timer->start(frame_ms);
			if (run_fwd)
				emit(dimen("Viewer running forward"));
			else
				emit(dimen("Viewer running backward"));
		}
		else
		{
			timer->setInterval(frame_ms);
		}
	}
	else
	{
		paused = true;
		timer->stop();
		emit dimen("Viewer paused");
	}
}

// toggle display of inactive landmarks
void SlamViz::toggleInactive(void)
{
  disp_inactive_lmrks = !disp_inactive_lmrks;
  update();
}

// toggle tracking of the robot's location with the camera
void SlamViz::togglePoseTrack(void)
{
  pose_track = !pose_track;
  update();
}

// toggle display of previous poses
void SlamViz::togglePrevPoses(void)
{
  disp_prev_poses = !disp_prev_poses;
  update();
}

// toggle projection mode
void SlamViz::toggleDisplay(void)
{
  mode = !mode;
  project(60,asp,dim);
  update();
}

// switch the texture used on the airplane
void SlamViz::switchTexture(void)
{
  plane->changeTexture();
  update();
}

//  Reset view angle
void SlamViz::reset(void)
{
  th = ph = 0;  //  Set parameter
  //shadowMap();
  update();     //  Request redisplay
}

//  Set dimension of the viewer
void SlamViz::setDIM(double DIM)
{
  dim = DIM;    //  Set parameter
  //emit dimen(QString::number(dim));
  //shadowMap();
  project(60,asp,dim);
  update();     //  Request redisplay
}

/******************************************************************/
/*************************  Mouse Events  *************************/
/******************************************************************/

//  Mouse pressed
void SlamViz::mousePressEvent(QMouseEvent* e)
{
  if (e->button() == Qt::RightButton)
    r_mouse = true;

  pos = e->pos();  //  Remember mouse location
}

//  Mouse released
void SlamViz::mouseReleaseEvent(QMouseEvent* e)
{
  if (e->button() == Qt::RightButton)
    r_mouse = false;
}

//  Mouse moved
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
  //shadowMap();
  update();                 //  Request redisplay
}

//  Mouse wheel
void SlamViz::wheelEvent(QWheelEvent* e)
{
  //  Zoom out
  if (e->delta()<0)
    setDIM(dim+1);
  //  Zoom in
  else if (dim>1)
    setDIM(dim-1);
  //  Signal to change dimension spinbox
}

/*******************************************************************/
/*************************  OpenGL Events  *************************/
/*******************************************************************/

//  Initialize the visualizer
void SlamViz::initializeGL()
{
  initializeOpenGLFunctions();
  initializeGLFunctions();
  glFuncs = QOpenGLContext::currentContext()->functions();
   
  setMouseTracking(true);  //  Ask for mouse events

  // add textures for airplane and skybox
  texture[0] = new QOpenGLTexture(QImage(QString("yellow_fabric.bmp")));
  texture[1] = new QOpenGLTexture(QImage(QString("metal.bmp")));
  texture[2] = new QOpenGLTexture(QImage(QString("bricks.bmp")));
  sky = new QOpenGLTexture(QImage(QString("sky2.jpg")));

  // load objects for airplane, landmarks, and smoke trail
  plane = new airplane(texture,3,glFuncs);
  star = new Star();
  smoke = new SmokeBB();

  glFuncs->glEnable(GL_DEPTH_TEST); //  Enable Z-buffer depth testing
  glFuncs->glDepthFunc(GL_LEQUAL);
  glFuncs->glPolygonOffset(4,0);

  emit dimen("Viewer running forward");

  //initShaders();
  //initMap();
}

// advance simulation forward or backward at timer interval
void SlamViz::timerEvent(void)
{
	timestep += (run_fwd? 1:-1);
	if (run_fwd && timestep == poses.size() - 1)
	{
		timer->stop();
		emit dimen("End of run reached");
	}
	else if (!run_fwd && timestep == 0)
	{
		timer->stop();
		emit dimen("Start of run reached");
	}
	else
	{
  	//shadowMap();
  	if (pose_track)
  	{
  		v_x = poses[timestep].trans[0];
  		v_y = poses[timestep].trans[1];
  		v_z = poses[timestep].trans[2];
  	}
  	else
  	{
  		v_x = v_y = v_z = 0.0;
  	}
  	updateGL();
	}
}

//  Window is resized
void SlamViz::resizeGL(int width, int height)
{
  //  Window aspect ration
  asp = (width && height) ? width / (float)height : 1;
  //  Viewport is whole screen
  glFuncs->glViewport(0,0,width,height);
  //  Set projection

  project(60,asp,dim);
}

//  Draw the window
void SlamViz::paintGL()
{
  double Ex = (-2)*dim*Sind(th)*Cosd(ph);
  double Ey = (2)*dim        *Sind(ph);
  double Ez = (2)*dim*Cosd(th)*Cosd(ph);

  //  Clear screen and Z-buffer
  glFuncs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glFuncs->glDisable(GL_LIGHTING);

  // set projection
  if (mode)
  {
		project(60,asp,dim);
    gluLookAt(Ex,Ey,Ez, 0,0,0, 0,Cosd(ph),0);
 		//gluLookAt(v_x,v_y,v_z, v_x-1,v_y,v_z, 0,1,0);
  }
  //  Orthogonal - set world orientation
  else
  {
    project(60,asp,dim);
    glRotatef(ph,1,0,0);
    glRotatef(th,0,1,0);
  }
   
  // track pose if pose tracking enabled
  glTranslated(-v_x,-v_y,-v_z);

  glColor3f(1,1,1);
   
  ball(Lpos[0],Lpos[1],Lpos[2],0.25);
  
  /*
  // shadow mapping code, not used in final

  shadow_shader->bind(); 
  shadow_shader->setUniformValue("tex", GLint(0));
  shadow_shader->setUniformValue("depth", GLint(1));

   
  glFuncs->glActiveTexture(GL_TEXTURE1);
  glTexGendv(GL_S, GL_EYE_PLANE, Svec);
  glTexGendv(GL_T, GL_EYE_PLANE, Tvec);
  glTexGendv(GL_R, GL_EYE_PLANE, Rvec);
  glTexGendv(GL_Q, GL_EYE_PLANE, Qvec);
  glFuncs->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_COMPARE_R_TO_TEXTURE);
  glFuncs->glActiveTexture(GL_TEXTURE0);
  */
  Scene(true);
  //shadow_shader->release();

  dispLandmarks();

  if (axes)
    drawAxes(2.0, true);
   
  if (disp_sky)
  {
    Sky(3.0*dim);
  }
  else
  {
    displayGrid(5);
  }

   
  if (disp_prev_poses)
  {
    float num_poses = 15.0;
    int pose_idx = timestep - 1;
    while (pose_idx > 0 && num_poses > 0)
    {
      if (poses[pose_idx].display_prev)
      {
        glPushMatrix();
        if (axes)
        {
          glRotated(-90.0,1.0,0.0,0.0);
          glMultMatrixf(glm::value_ptr(poses[pose_idx].T_WS));
          drawAxes(0.5,false);
        }
        else
        {
          double max_age = 20.0;
          double age = poses[timestep].timestamp - poses[pose_idx].timestamp;
          age = std::max(age, 1.0);
          if (poses[timestep].timestamp - poses[pose_idx].timestamp < max_age)
          {
            float x = poses[pose_idx].T_WS[3][0];
            float z = -poses[pose_idx].T_WS[3][1];
            float y = poses[pose_idx].T_WS[3][2];
            glTranslatef(x,y,z);

            smoke->DrawSmoke(Ex+v_x,Ey+v_y,Ez+v_z, 
                             v_x,v_y,v_z, 
                             0.05*max_age/age);
          }
          num_poses--;
        }
        glPopMatrix();
      }
      pose_idx--;
    }
  }
  //  Done
  glFlush();
}

// Draw vertex in polar coordinates with normal
void SlamViz::Vertex(double th,double ph)
{
  double x = Sind(th)*Cosd(ph);
  double y = Cosd(th)*Cosd(ph);
  double z =          Sind(ph);
  //  For a sphere at the origin, the position
  //  and normal vectors are the same
  glNormal3d(x,y,z);
  glVertex3d(x,y,z);
}

// draw a ball, really only used for the light location 
void SlamViz::ball(double x,double y,double z,double r)
{
  int th,ph;
  int inc = 30;
  //  Save transformation
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y,z);
  glScaled(r,r,r);
  //  White ball
  glColor3f(1,1,1);

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

//  Set projection
void SlamViz::project(double fov, double asp, double dim)
{
   //  Orthogonal projection to dim
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   //if (asp>1)
   //   glOrtho(-dim*asp, +dim*asp, -dim, +dim, -8*dim, + 8*dim);
   if (mode)
      gluPerspective(fov,asp,dim/16,16*dim);
   else
      glOrtho(-asp*dim, asp*dim, -dim, +dim, -dim*8, +8.0*dim);

   //  Back to model view
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

// draw the grid used for the gridworld display
// gridlines drawn with spacing D
void SlamViz::displayGrid(double D)
{
  double limit = 2.0*dim;

  glFuncs->glDisable(GL_LIGHTING);
  glBegin(GL_LINES);
  glColor3f(1.0,0.0,0.0);

  glVertex3d(-limit,0.0,0.0);
  glVertex3d(limit,0.0,0.0);
  glVertex3d(0.0,0.0,-limit);
  glVertex3d(0.0,0.0,limit);
  glColor3f(1.0,1.0,1.0);
  
  for (int i = -limit/D; i <= limit/D; i++)
  {
    if (i != 0)
    {
      glVertex3d(-limit,0.0,double(D*i));
      glVertex3d(limit,0.0,double(D*i));
    }
  }

  for (int i = -limit/D; i <= limit/D; i++)
  {
    if (i != 0)
    {
      glVertex3d(D*i,0.0,-limit);
      glVertex3d(D*i,0.0,limit);
    }
  }
  glEnd();
}

// draw the skybox with dimension D
void SlamViz::Sky(double D)
{
  glColor3f(1,1,1);
  glPushMatrix();

  // draw skybox centered at view center
  glTranslated(v_x,v_y,v_z);
  glFuncs->glEnable(GL_TEXTURE_2D);
  glFuncs->glEnable(GL_CULL_FACE);
  glFuncs->glEnable(GL_LIGHTING);

  //  Sides
  sky->bind();
  glBegin(GL_QUADS);
  glNormal3d(0.0,0.0,1.0);
  glTexCoord2f(0.25,0.6667); glVertex3f(-D,-D,-D);
  glTexCoord2f(0.5,0.6667); glVertex3f(+D,-D,-D);
  glTexCoord2f(0.5,0.3333); glVertex3f(+D,+D,-D);
  glTexCoord2f(0.25,0.3333); glVertex3f(-D,+D,-D);

  glNormal3d(-1.0,0.0,0.0);
  glTexCoord2f(0.5,0.6667); glVertex3f(+D,-D,-D);
  glTexCoord2f(0.75,0.6667); glVertex3f(+D,-D,+D);
  glTexCoord2f(0.75,0.3333); glVertex3f(+D,+D,+D);
  glTexCoord2f(0.5,0.3333); glVertex3f(+D,+D,-D);

  glNormal3d(0.0,0.0,-1.0);
  glTexCoord2f(0.75,0.6667); glVertex3f(+D,-D,+D);
  glTexCoord2f(1.0,0.6667); glVertex3f(-D,-D,+D);
  glTexCoord2f(1.0,0.3333); glVertex3f(-D,+D,+D);
  glTexCoord2f(0.75,0.3333); glVertex3f(+D,+D,+D);

  glNormal3d(1.0,0.0,0.0);
  glTexCoord2f(0.0,0.6667); glVertex3f(-D,-D,+D);
  glTexCoord2f(0.25,0.6667); glVertex3f(-D,-D,-D);
  glTexCoord2f(0.25,0.3333); glVertex3f(-D,+D,-D);
  glTexCoord2f(0.0,0.3333); glVertex3f(-D,+D,+D);
  glEnd();

  //  Top and bottom
  glBegin(GL_QUADS);
  glNormal3d(0.0,-1.0,0.0);
  glTexCoord2f(0.5,0.3334); glVertex3f(+D,+D,-D);
  glTexCoord2f(0.5,0.0); glVertex3f(+D,+D,+D);
  glTexCoord2f(0.25,0.0); glVertex3f(-D,+D,+D);
  glTexCoord2f(0.25,0.3334); glVertex3f(-D,+D,-D);

  glNormal3d(0.0,1.0,0.0);
  glTexCoord2f(0.25,1.0); glVertex3f(-D,-D,+D);
  glTexCoord2f(0.5,1.0); glVertex3f(+D,-D,+D);
  glTexCoord2f(0.5,0.6667); glVertex3f(+D,-D,-D);
  glTexCoord2f(0.25,0.6667); glVertex3f(-D,-D,-D);
  glEnd();
  sky->release();
  glFuncs->glDisable(GL_TEXTURE_2D);
  glFuncs->glDisable(GL_CULL_FACE);
  glPopMatrix();
}

// read poses in from log file
bool SlamViz::readPoses()
{
	std::ifstream *pose_file = new std::ifstream();
  pose_file->open("pose_log.txt");

  if (!pose_file)
  	return false;

  std::string line;
  unsigned int last_prev_idx = 0;
  while (std::getline(*pose_file,line))
  {
    std::istringstream ss(line);
    std::string token;
    std::getline(ss, token, ' ');
    Pose cur_pose;
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

    cur_pose.trans[0] = translation[0];
    cur_pose.trans[1] = translation[2];
    cur_pose.trans[2] = -translation[1];

    cur_pose.display_prev = shouldDisplayPrev(last_prev_idx, cur_pose);
      
    if (cur_pose.display_prev)
    	last_prev_idx = poses.size();

    poses.push_back(cur_pose);
   }
  return true;
}

// read landmarks in from log file
bool SlamViz::readLmrks()
{
	std::ifstream *lmrk_file = new std::ifstream();
  lmrk_file->open("lmrk_log.txt");

  if (!lmrk_file)
  	return false;

  std::string line;
  int lmrk_timestep = 0;
  while (std::getline(*lmrk_file, line))
  {
    // insert new landmarks
    std::map<unsigned long, Landmark> active_lmrks;

    unsigned int stamp = std::stod(line);
    std::getline(*lmrk_file, line);

    // get landmarks for the current timestep
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

      // get current landmark location
      for (int i = 0; i < 3; i++)
      {
        std::getline(ss, token, ' ');
        lmrk.point[i] = scale_factor*std::stof(token);
      }
      
      active_lmrks.insert(std::pair<unsigned long, Landmark>(id, lmrk));

      std::getline(*lmrk_file, line);
    }
    // iterate over active landmarks from last timestep
    // add landmarks that aren't active in current timestep 
    // to marginalized landmarks
    if (lmrk_timestep > 0)
    {
	    for (std::map<unsigned long, Landmark>::iterator it = lmrks[lmrk_timestep-1].begin();
	         it != lmrks[lmrk_timestep-1].end(); it++)
	    {
	      if (active_lmrks.find(it->first) == active_lmrks.end())
	      {
	        it->second.marginalized_timestep = lmrk_timestep;
	        inactive_lmrks.insert(std::pair<unsigned long, Landmark>(it->first, it->second));
	      }
	    }
  	}
  	lmrks.push_back(active_lmrks);
    lmrk_timestep++;
  }
  return true;
}

// draw a set of xyz axes 
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

// decide if pose should be displayed when previous poses are displayed
// returns true if current pose is 5m from the last displayed pose
bool SlamViz::shouldDisplayPrev(int last_index, Pose cur_pose)
{
  if (poses.size() == 0)
  {
    return true;
  }
  else
  {
    Pose prev_pose = poses[last_index];

    // get translation component from current pose and
    // last pose in previous pose vector
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 cur_trans;
    glm::vec3 prev_trans;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(cur_pose.T_WS, scale, rotation, cur_trans, skew, perspective);
    glm::decompose(prev_pose.T_WS, scale, rotation, prev_trans, skew, perspective);

    // calculate the magnitude of the translation
    // between the current pose and last pose
    double dist = sqrt(glm::length2(cur_trans - prev_trans));

    // if distance from last pose is greater than threshold, 
    // return true otherwise return false
    if (dist > 0.5)
    	return true;
    else
    	return false;
  }
}

// initialize shader program
// not used in final submission
void SlamViz::initShaders()
{
  shadow_shader = new QOpenGLShaderProgram(this);
  // compile shader for shadowing
  if (!shadow_shader->addShaderFromSourceFile(QOpenGLShader::Vertex, "shadow.vert"))
    close();
  if (!shadow_shader->addShaderFromSourceFile(QOpenGLShader::Fragment, "shadow.frag"))
    close();
  if (!shadow_shader->link())
    close();
}

// initialize shadow map
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
  glFuncs->glActiveTexture(GL_TEXTURE1);
  glFuncs->glGenTextures(1,&shadowtex);
  glFuncs->glBindTexture(GL_TEXTURE_2D, shadowtex);
  glFuncs->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowdim, shadowdim, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

  glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
  //  Set texture mapping to clamp and linear interpolation
  glFuncs->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
  glFuncs->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
  glFuncs->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glFuncs->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  //  Set automatic texture generation mode to Eye Linear
  glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
  glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
  glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
  glTexGeni(GL_Q,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);

  // Switch back to default textures
  glFuncs->glActiveTexture(GL_TEXTURE0);

  // Attach shadow texture to frame buffer
  glFuncs->glGenFramebuffers(1,&framebuf);
  glFuncs->glBindFramebuffer(GL_FRAMEBUFFER,framebuf);
  glFuncs->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowtex, 0);
  //  Don't write or read to visible color buffer
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  //  Make sure this all worked
  if (glFuncs->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) Fatal("Error setting up frame buffer\n");
  glFuncs->glBindFramebuffer(GL_FRAMEBUFFER,0);

  ErrCheck("InitMap");

  shadowMap();
}

// draw shadow map
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
  glFuncs->glColorMask(0,0,0,0);
  glFuncs->glEnable(GL_POLYGON_OFFSET_FILL);
   
  Light(false);

  Ldist = sqrt(Lpos[0]*Lpos[0] + Lpos[1]*Lpos[1] + Lpos[2]*Lpos[2]);
  if(Ldist < 1.1*Dim) Ldist = 1.1*Dim;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(114.6*atan(Dim/Ldist),1,Ldist-Dim,Ldist+Dim);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(Lpos[0],Lpos[1],Lpos[2], v_x,v_y,v_z, 0,1,0);
  glFuncs->glViewport(0,0,shadowdim,shadowdim);
   
  glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, framebuf);
  glClear(GL_DEPTH_BUFFER_BIT);

  Scene(false);

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
  glFuncs->glDisable(GL_POLYGON_OFFSET_FILL);
  glPopAttrib();
  glPopMatrix();
  glFuncs->glBindFramebuffer(GL_FRAMEBUFFER,0);
}

// set and activate light
void SlamViz::Light(bool light)
{
  //  Set light position
  Lpos[0] = 2.5*dim;
  Lpos[1] = 2.5*dim;
  Lpos[2] = 0;
  Lpos[3] = 1;

  //  Enable lighting
  if (light)
  {
    float Med[]  = {0.3,0.3,0.3,1.0};
    float High[] = {1.0,1.0,1.0,1.0};
    //  Enable lighting with normalization
    glFuncs->glEnable(GL_LIGHTING);
    glFuncs->glEnable(GL_NORMALIZE);
    //  glColor sets ambient and diffuse color materials
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glFuncs->glEnable(GL_COLOR_MATERIAL);
    //  Enable light 0
    glFuncs->glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0,GL_POSITION,Lpos);
    glLightfv(GL_LIGHT0,GL_AMBIENT,Med);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,High);
  }
  else
  {
    glFuncs->glDisable(GL_LIGHTING);
    glFuncs->glDisable(GL_COLOR_MATERIAL);
    glFuncs->glDisable(GL_NORMALIZE);
  }
}

// draw scene including light robot, and landmarks
void SlamViz::Scene(bool light)
{
  Light(light);

  if (light)
  {
    glFuncs->glEnable(GL_TEXTURE_2D);
  }
   
  glPushMatrix();
  //  Draw scene

  glRotated(-90.0,1.0,0.0,0.0);
  glMultMatrixf(glm::value_ptr(poses[timestep].T_WS));

  plane->drawAirplane(0,0,0,
                      0,0,1,
                      1,0,0);

  glPopMatrix();

  dispLandmarks();
      
  if (light) 
     glFuncs->glDisable(GL_TEXTURE_2D);
  // prior poses, axes and skybox don't cast shadows, so return
  // here if not doing lighting
  if (!light) 
  {
    return;
  }   
}

// display landmarks as stars
// landmark always face toward robot's current location
void SlamViz::dispLandmarks()
{
  for (std::map<unsigned long, Landmark>::iterator it = lmrks[timestep].begin();
    it != lmrks[timestep].end(); it++)
  {
    if (it->second.quality >= lmrk_lwr_bound)
    {
      glPushMatrix();
      glRotated(-90.0,1.0,0.0,0.0);
      double x = it->second.point[0];
      double y = it->second.point[1];
      double z = it->second.point[2];
      star->drawStar(x,y,z, x-v_x,y-v_y,z-v_z, 1.,0.,0., it->second.quality);
      glPopMatrix();
    }
  }
  if (disp_inactive_lmrks)
  {
    for (std::map<unsigned long, Landmark>::iterator it = inactive_lmrks.begin();
      it != inactive_lmrks.end(); it++)
    {
      if (it->second.marginalized_timestep < timestep
       	&& it->second.quality >= lmrk_lwr_bound)
      {
        glPushMatrix();
        glRotated(-90.0,1.0,0.0,0.0);
        double x = it->second.point[0];
        double y = it->second.point[1];
        double z = it->second.point[2];
        star->drawStar(x,y,z, x-poses[timestep].trans[0],
           	y-poses[timestep].trans[1],z-poses[timestep].trans[2], 
           	1.,0.,0., it->second.quality);;
        glPopMatrix();
      }
    }
  }
}



