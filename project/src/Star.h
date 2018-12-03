#ifndef STAR_H
#define STAR_H

#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <QOpenGLTexture>

class Star
{
public:
	Star();
	void drawStar(double cx, double cy, double cz, 
				  double dx, double dy, double dz,
				  double ux, double uy, double uz, double scale);
private:
	std::vector<glm::vec3> star_vertices;
	std::vector<glm::vec2> star_uvs;
	std::vector<glm::vec3> star_normals;
	QOpenGLTexture *star_tex;

	void loadOBJ(const char *path, std::vector<glm::vec3> &out_vertices,
		std::vector<glm::vec2> &out_uvs, std::vector<glm::vec3> &out_normals);
};

#endif