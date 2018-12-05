#include "SmokeBB.h"

SmokeBB::SmokeBB()
{
	smoke_tex = new QOpenGLTexture(QImage(QString("smoke_tex.png")));
}

void SmokeBB::DrawSmoke(float cam_x, float cam_y, float cam_z,
			  	   		float obj_pos_x, float obj_pos_y, 
			  	   		float obj_pos_z, float scale)
{
	float look_at[3], obj_to_cam_proj[3], obj_to_cam[3], up_aux[3];
	float angle_cosine;

	glPushMatrix();
	

	// calculate vector from local origin to camera projected in xz
	obj_to_cam_proj[0] = cam_x - obj_pos_x;
	obj_to_cam_proj[1] = 0;
	obj_to_cam_proj[2] = cam_z - obj_pos_z;

	// original look-at vector for object in world coordinates
	look_at[0] = 0;
	look_at[1] = 0;
	look_at[2] = 1;

	// normalize obj_to_cam_proj
	Normalize(obj_to_cam_proj);

	// get cross product of look_at and obj_to_cam_proj
	CrossProduct(look_at, obj_to_cam_proj, up_aux);

	// get cosine of angle between look_at and obj_to_cam_proj
	angle_cosine = InnerProduct(look_at, obj_to_cam_proj);

	// perform rotation
	if ((angle_cosine < 0.9999) && (angle_cosine > -0.9999))
		glRotatef(acos(angle_cosine)*180.0/3.1415, 
					up_aux[0], up_aux[1], up_aux[2]);

	// get vector from object to camera in 3D
	obj_to_cam[0] = cam_x - obj_pos_x;
	obj_to_cam[1] = cam_y - obj_pos_y;
	obj_to_cam[2] = cam_z - obj_pos_z;

	Normalize(obj_to_cam);

	// get cosine between xz plane and obj_to_cam
	angle_cosine = InnerProduct(obj_to_cam_proj, obj_to_cam);

	// tilt upward
	if ((angle_cosine < 0.9999) && (angle_cosine > -0.9999))
	{
		if (obj_to_cam[1] < 0)
			glRotatef(acos(angle_cosine)*180.0/3.1415,1,0,0);
		else
			glRotatef(acos(angle_cosine)*180.0/3.1415,-1,0,0);
	}
	//glTranslated(obj_pos_x, obj_pos_y, obj_pos_z);
	
	DrawObject(scale);

	glPopMatrix();
}

void SmokeBB::DrawObject(float scale)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	smoke_tex->bind();
	glScalef(scale,scale,scale);
	glColor4f(1.0,1.0,1.0,1.0);
	//glColor3f(1.0,1.0,1.0);

	glBegin(GL_POLYGON);
	glNormal3f(0.0,0.0,1.0);
	for (double th = 0; th <= 360; th +=22.5)
	{
		glTexCoord2f(0.5+0.45*Cosd(th), 0.5+0.45*Sind(th));
		glVertex3d(0.5*Cosd(th),0.5*Sind(th),0.0);
	}
	/*
	glBegin(GL_QUADS);
	glNormal3f(0.0,0.0,1.0);
	glTexCoord2f(0.0,0.0); glVertex3d(-0.5,-0.5,0.0);
	glTexCoord2f(1.0,0.0); glVertex3d(0.5,-0.5,0.0);
	glTexCoord2f(1.0,1.0); glVertex3d(0.5,0.5,0.0);
	glTexCoord2f(0.0,1.0); glVertex3d(-0.5,0.5,0.0);
	*/
	glEnd();
	smoke_tex->release();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

void SmokeBB::Normalize(float *a)
{
	float mag_a = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
	a[0] /= mag_a;
	a[1] /= mag_a;
	a[2] /= mag_a;
}

void SmokeBB::CrossProduct(float *a, float *b, float *c)
{
	c[0] = a[1]*b[2] - a[2]*b[1];
	c[1] = a[2]*b[0] - a[0]*b[2];
	c[2] = a[0]*b[1] - a[1]*b[0];
}

float SmokeBB::InnerProduct(float *a, float *b)
{
	float c = a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
	return c;
}