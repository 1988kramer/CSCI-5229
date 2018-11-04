//
// OpenGL SLAM visualization widget
//

#ifndef SLAMVIZ_H
#define SLAMVIZ_H

#include <QGLWidget>
#include <QString>
#include <QOpenGLTexture>
#include "airplane.h"
#include "CSCIx229.h"
#include <QOpenGLFunctions>
#include <iostream>

QT_FORWARD_DECLARE_CLASS(QOpenGLTexture);

class SlamViz : public QGLWidget
{
Q_OBJECT
private:
	int th;
	int ph;
	bool mouse;
	bool axes;
	bool light;
	bool mode; 
	QPoint pos;
	double dim;
	double asp;
	double x0,y0,z0;  //  Start position
	double ylight;
	int smooth;
	int ambient, diffuse, specular, distance, zh,
			local, emission, shiny, inc;
	airplane* plane;
	QOpenGLTexture *texture[3];
	QTimer* timer;

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

signals:
	void angles(QString text); // Signal for display angles
	void dimen(double dim);    // Signal for display dimensions

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
	void project();
};

#endif