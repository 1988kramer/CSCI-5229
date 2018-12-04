#include "Star.h"

Star::Star()
{
	star_tex = new QOpenGLTexture(QImage(QString("star_tex.jpg")));
	loadOBJ("star.obj", star_vertices, star_uvs, star_normals);
}

void Star::loadOBJ(const char *path, std::vector<glm::vec3> &out_vertices,
		std::vector<glm::vec2> &out_uvs, std::vector<glm::vec3> &out_normals)
{
	std::ifstream obj_file;
	obj_file.open(path);
	std::string line;

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	while(std::getline(obj_file, line))
	{
		std::istringstream ss(line);
		std::string token;
		std::getline(ss, token, ' ');

		// if line describes a vertex
		if (token == "v")
		{
			glm::vec3 vertex;
			for (int i = 0; i < 3; i++)
			{
				std::getline(ss, token, ' ');
				vertex[i] = std::stof(token);
			}
			temp_vertices.push_back(vertex);
		}
		// if line describes a uv
		else if (token == "vt")
		{
			glm::vec2 uv;
			for (int i = 0; i < 2; i++)
			{
				std::getline(ss, token, ' ');
				uv[i] = std::stof(token);
			}
			temp_uvs.push_back(uv);
		}
		// if line describes a normal
		else if (token == "vn")
		{
			glm::vec3 normal;
			for (int i = 0; i < 3; i++)
			{
				std::getline(ss, token, ' ');
				normal[i] = std::stof(token);
			}
			temp_normals.push_back(normal);
		}
		else if (token == "f")
		{
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			for (int i = 0; i < 3; i++)
			{
				std::getline(ss, token, ' ');
				std::istringstream ss2(token);
				std::string index;
				std::getline(ss2, index, '/');
				vertexIndex[i] = std::stoul(index);
				std::getline(ss2, index, '/');
				uvIndex[i] = std::stoul(index);
				std::getline(ss2, index, '/');
				normalIndex[i] = std::stoul(index);
			}
			std::cerr << std::endl;
			for (int i = 0; i < 3; i++)
			{
				vertexIndices.push_back(vertexIndex[i]);
				uvIndices.push_back(uvIndex[i]);
				normalIndices.push_back(normalIndex[i]);
			}
		}
	}

	// process data read from file
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		out_vertices.push_back(vertex);

		unsigned int uvIndex = uvIndices[i];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		out_uvs.push_back(uv);

		unsigned int normalIndex = normalIndices[i];
		glm::vec3 normal = temp_normals[normalIndex - 1];
		out_normals.push_back(normal);
	}
}

void Star::drawStar(double cx, double cy, double cz, 
								double dx, double dy, double dz,
								double ux, double uy, double uz,
								double scale)
{
	//  Unit vector for facing direction
  double D0 = sqrt(dx*dx+dy*dy+dz*dz);
  double X0 = dx/D0;
  double Y0 = dy/D0;
  double Z0 = dz/D0;
  //  Unit vector in "up" direction
  double D1 = sqrt(ux*ux+uy*uy+uz*uz);
  double X1 = ux/D1;
  double Y1 = uy/D1;
  double Z1 = uz/D1;
  //  Cross product gives the third vector
  double X2 = Y0*Z1-Y1*Z0;
  double Y2 = Z0*X1-Z1*X0;
  double Z2 = X0*Y1-X1*Y0;
  //  Rotation matrix
  double mat[16];
  mat[0] = X0;   mat[4] = X1;   mat[ 8] = X2;   mat[12] = 0;
  mat[1] = Y0;   mat[5] = Y1;   mat[ 9] = Y2;   mat[13] = 0;
  mat[2] = Z0;   mat[6] = Z1;   mat[10] = Z2;   mat[14] = 0;
  mat[3] =  0;   mat[7] =  0;   mat[11] =  0;   mat[15] = 1;

  //float em[] = {1,1,1,1};

  // save current transforms
  glPushMatrix();

  // offset, scale and rotate
  glTranslated(cx,cy,cz);
  glRotated(90,0.,0.,1.);
  glMultMatrixd(mat);
  glScaled(scale, scale, scale);



  glEnable(GL_TEXTURE_2D);
  //glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,em);
  star_tex->bind();
  glBegin(GL_TRIANGLES);
  for (int i = 0; i < star_vertices.size(); i++)
  {
  	glNormal3f(star_normals[i][0],star_normals[i][1],star_normals[i][2]);
  	glTexCoord2f(star_uvs[i][0],star_uvs[i][1]);
  	glVertex3f(star_vertices[i][0],star_vertices[i][1],star_vertices[i][2]);
  }
  glEnd();
  star_tex->release();
  glDisable(GL_TEXTURE_2D);

  glPopMatrix();
}