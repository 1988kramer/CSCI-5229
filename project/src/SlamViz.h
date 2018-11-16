//
// OpenGL SLAM visualization widget
//

#ifndef SLAMVIZ_H
#define SLAMVIZ_H

#define GLM_ENABLE_EXPERIMENTAL

#include <QGLWidget>
#include <QString>
#include <QOpenGLTexture>
#include "airplane.h"
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
#include <glm/matrix.hpp>


QT_FORWARD_DECLARE_CLASS(QOpenGLTexture);

typedef struct Pose
{
	glm::mat4 T_WS;
	double timestamp;
} Pose;

class SlamViz : public QGLWidget
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
	QPoint pos;
	double dim;
	double asp;
	double x0,y0,z0;  //  Start position
	double x,y,z; // current view center
	double ylight;
	double cur_time;
	double last_time;
	double last_stamp;
	int smooth;
	int ambient, diffuse, specular, distance, zh,
			local, emission, shiny, inc;
	airplane* plane;
	QOpenGLTexture *texture[3];
	QOpenGLTexture *sky[3];
	QTimer* timer;
	std::ifstream* pose_file;
	Pose cur_pose;
	std::vector<Pose> prev_poses;

public:
	SlamViz(QWidget* parent=0);
	QSize sizeHint() const {return QSize(400,400);}

public slots:
	void reset(void);  // Reset view angles and zoom 
	void toggleAxes(void);
	void toggleLight(void);
	void toggleDisplay(void);
  	void setDIM(double DIM);    //  Slot to set dim
  	void switchTexture(void);
  	void timerEvent(void);
  	void toggleSky(void);

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
	void project();
	void readPose();
	void drawAxes(double len, bool draw_labels);
	void addToPrevPoses();
};

#endif