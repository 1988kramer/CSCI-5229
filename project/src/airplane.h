//
// draws airplane at specified location and orientation
//

#ifndef AIRPLANE_H
#define AIRPLANE_H

#include "CSCIx229.h"
#include <QOpenGLTexture>

class airplane
{
public:
	airplane(QOpenGLTexture **textures, int num_tex); // constructor
	void drawAirplane(double x, double y, double z,
										double dx, double dy, double dz,
										double ux, double uy, double uz);
	void changeTexture();


private:
	QOpenGLTexture** texture;
	int ntex = 0;
	int num_textures;


	void Vertex(double th, double ph);
	void pointOnCircle(double th, double r, double c_x, double c_y, double c_z);
	void pointOnCircle2(double th, double r, double c_x, double c_y, double c_z,
		double *px, double *py, double *pz);
	void getCowlNorms(double aft_x, double aft_y, double fwd_x, double fwd_y, double th);
	void crossProduct(double a_i, double a_j, double a_k,
  	double b_i, double b_j, double b_k,
  	double c_i, double c_j, double c_k,
  	double *n_i, double *n_j, double *n_k);
	void crossProductNorm(double a_i, double a_j, double a_k,
  	double b_i, double b_j, double b_k,
  	double c_i, double c_j, double c_k);
	void drawWindow(int horiz_seg, int vert_seg, double start_x,
    double start_y, double start_z, double end_x,
    double end_y, double end_z);
	void drawFuselage();
	void drawWing();
	void drawVStab();
	void drawHStab();
};

#endif