#ifndef SMOKEBB_H
#define SMOKEBB_H

#include "CSCIx229.h"
#include <QOpenGLTexture>
#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

class SmokeBB
{
public:
	SmokeBB();
	void DrawSmoke(float cam_x, float cam_y, float cam_z,
			  	   float obj_pos_x, float obj_pos_y, 
			  	   float obj_pos_z, float scale);
private:
	QOpenGLTexture *smoke_tex;
	void Normalize(float *a);
	void CrossProduct(float *a, float *b, float *c);
	float InnerProduct(float *a, float *b);
	void DrawObject(float scale);
};

#endif