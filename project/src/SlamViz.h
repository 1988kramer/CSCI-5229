//
// OpenGL SLAM visualization widget
//

#ifndef SLAMVIZ_H
#define SLAMVIZ_H

#define GLM_ENABLE_EXPERIMENTAL


#include <QGLWidget>
#include <QGLFunctions>
#include <QString>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>

//#include <GL/gl.h>

#include "airplane.h"
#include "Star.h"
#include "SmokeBB.h"
#include "CSCIx229.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/matrix.hpp>


QT_FORWARD_DECLARE_CLASS(QOpenGLTexture);

typedef struct Pose
{
	glm::mat4 T_WS;
	glm::vec3 trans;
	double timestamp;
	bool display_prev;
} Pose;

typedef struct Landmark
{
	glm::vec3 point;
	double timestamp;
	double quality;
	int marginalized_timestep;
} Landmark;

class SlamViz : public QGLWidget, protected QGLFunctions, protected QOpenGLFunctions
{
Q_OBJECT
private:
	int th;
	int ph;
	int fov;
	bool r_mouse, l_mouse;
	bool axes;
	bool light;
	bool mode; 
	bool disp_sky;
	bool disp_inactive_lmrks;
	bool pose_track;
	bool disp_prev_poses;
	double lmrk_lwr_bound;
	QPoint pos;
	double dim;
	double asp;
	double v_x,v_y,v_z; // current view center
	double scale_factor;
	double Svec[4];
	double Tvec[4];
	double Rvec[4];
	double Qvec[4];
	float Lpos[4];
	int shadowdim;
	unsigned int framebuf;
	airplane* plane;
	Star* star;
	SmokeBB* smoke;
	QOpenGLTexture *texture[3];
	QOpenGLTexture *sky;
	QTimer* timer;
	std::vector<Pose> poses;
	std::vector<std::map<unsigned long, Landmark>> lmrks;
	std::map<unsigned long, Landmark> inactive_lmrks;
	unsigned long timestep;
	int frame_ms; // time a single frame is displayed (inverse of fps)
	bool paused, run_fwd; // is visualization paused?

	QOpenGLShaderProgram *shadow_shader;
	QOpenGLFunctions *glFuncs;


public:
	SlamViz(QWidget* parent=0);
	QSize sizeHint() const {return QSize(512,512);}

public slots:
	void reset(void);  // Reset view angles and zoom 
	void toggleAxes(void);
	void toggleDisplay(void);
  void setDIM(double DIM);    //  Slot to set dim
  void switchTexture(void);
  void timerEvent(void);
  void toggleSky(void);
  void setLmrkDispBound(double bound);
  void setFPS(int fps);
  void toggleInactive(void);
  void togglePoseTrack(void);
  void togglePrevPoses(void);

signals:
	void angles(QString text); // Signal for display angles
	void dimen(QString text);    // Signal for display dimensions

protected:
	void initializeGL();											// Initialize widget
	void resizeGL(int width, int height);			// Resize widget
	void paintGL();														// Draw widget
	void mousePressEvent(QMouseEvent*);				// Mouse pressed
	void mouseReleaseEvent(QMouseEvent*);			// Mouse released
	void mouseMoveEvent(QMouseEvent*);				// Mouse moved
	void wheelEvent(QWheelEvent*);						// Mouse wheel
private:
	void ball(double x,double y,double z,double r);
	void Vertex(double th, double ph);
	void Sky(double D);
	void displayGrid(double D);
	void project(double fov, double asp, double dim);
	bool readPoses();
	bool readLmrks();
	void drawAxes(double len, bool draw_labels);
	bool shouldDisplayPrev(int last_index, Pose cur_pose);

	void initShaders();
	void initMap();
	void shadowMap(void);
	void Light(bool light);
	void Scene(bool light);
	void dispLandmarks();
};

#endif