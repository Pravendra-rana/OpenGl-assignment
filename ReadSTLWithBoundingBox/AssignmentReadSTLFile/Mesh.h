#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>
#include "common/includes/GL/glew.h"
#include "common/includes/glm/glm.hpp"
using namespace std;
struct Vertex
{
	glm::vec3 position;
	glm::vec2 textureCoord;
};
class Mesh
{
public:
	Mesh();
	~Mesh();

	bool loadSTL(const string& filename);
	void draw();
	void drawBoundingBox();
	void boundingbox();
private:
	void initBuffers();
	bool mLoaded;
	vector<Vertex>mVertices;
	vector<Vertex>bVertices;
	GLuint mVAO, mVAO2, mVBO, mVBO2;
	//GLuint VAO, VAO2, VBO, VBO2;
	GLfloat
		min_x, max_x,
		min_y, max_y,
		min_z, max_z;
};
#endif
