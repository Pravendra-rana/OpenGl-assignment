// Basic Mesh class
//-----------------------------------------------------------------------------
#include "Mesh.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "common/includes/glm/glm.hpp"
#include "common/includes/glm/gtc/matrix_transform.hpp"


Mesh::Mesh() :mLoaded(false)
{
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &mVAO);
	glDeleteVertexArrays(1, &mVBO);

}

bool Mesh::loadSTL(const std::string& filename)
{
	std::vector<glm::vec3> tempVertices;
	//std::vector<glm::vec2> tempUVs;

	if (filename.find(".stl") != std::string::npos)
	{
		std::ifstream fin(filename, std::ios::in);
		if (!fin)
		{
			std::cerr << "Cannot open " << filename << std::endl;
			return false;
		}

		std::cout << "Loading STL file " << filename << " ..." << std::endl;
		std::string lineBuffer;
		while (std::getline(fin, lineBuffer))
		{
			int pos = lineBuffer.find("vertex");
			if (pos != std::string::npos)
			{
				if (lineBuffer.substr(pos, 7) == "vertex ")  //4, 7
				{
					std::istringstream v(lineBuffer.substr(pos + 7));   //10
					glm::vec3 vertex;
					v >> vertex.x;
					v >> vertex.y;
					v >> vertex.z;
					tempVertices.push_back(vertex);
					std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
				}
			}
		}
		fin.close();

		// For each vertex of each triangle
		for (unsigned int i = 0; i < tempVertices.size(); i++)
		{
			// Get the attributes using the indices
			glm::vec3 vertex = tempVertices[i];
			//glm::vec2 uv = tempUVs[uvIndices[i] - 1];

			Vertex meshVertex;
			meshVertex.position = vertex;
			//meshVertex.texCoords = uv;

			mVertices.push_back(meshVertex);
		}


		// Calculate the minimum and maximum coordinates
		glm::vec3 minCoords = tempVertices[0];
		glm::vec3 maxCoords = tempVertices[0];

		for (const auto& vertex : tempVertices)
		{
			minCoords = glm::min(minCoords, vertex);
			maxCoords = glm::max(maxCoords, vertex);
		}

		// Create the bounding box vertices
		std::vector<glm::vec3> boundingBoxVertices = {

			glm::vec3(minCoords.x, minCoords.y, minCoords.z),       // BOT BOX
			glm::vec3(maxCoords.x, minCoords.y, minCoords.z),

			glm::vec3(maxCoords.x, minCoords.y, minCoords.z),
			glm::vec3(maxCoords.x, minCoords.y, maxCoords.z),

			glm::vec3(maxCoords.x, minCoords.y, maxCoords.z),
			glm::vec3(minCoords.x, minCoords.y, maxCoords.z),

			glm::vec3(minCoords.x, minCoords.y, maxCoords.z),
			glm::vec3(minCoords.x, minCoords.y, minCoords.z),             // BOT BOX 

			glm::vec3(minCoords.x, maxCoords.y, minCoords.z),            //TOP BOX
			glm::vec3(maxCoords.x, maxCoords.y, minCoords.z),

			glm::vec3(maxCoords.x, maxCoords.y, minCoords.z),
			glm::vec3(maxCoords.x, maxCoords.y, maxCoords.z),

			glm::vec3(maxCoords.x, maxCoords.y, maxCoords.z),
			glm::vec3(minCoords.x, maxCoords.y, maxCoords.z),

			glm::vec3(minCoords.x, maxCoords.y, maxCoords.z),
			glm::vec3(minCoords.x, maxCoords.y, minCoords.z),             // TOP BOX

			glm::vec3(minCoords.x, minCoords.y, minCoords.z),             // LEFT BOX
			glm::vec3(minCoords.x, minCoords.y, maxCoords.z),

			glm::vec3(minCoords.x, minCoords.y, maxCoords.z),
			glm::vec3(minCoords.x, maxCoords.y, maxCoords.z),

			glm::vec3(minCoords.x, maxCoords.y, maxCoords.z),
			glm::vec3(minCoords.x, maxCoords.y, minCoords.z),

			glm::vec3(minCoords.x, maxCoords.y, minCoords.z),
			glm::vec3(minCoords.x, minCoords.y, minCoords.z),            // LEFT BOX


			glm::vec3(maxCoords.x, minCoords.y, minCoords.z),             // RIGHT BOX
			glm::vec3(maxCoords.x, minCoords.y, maxCoords.z),
			glm::vec3(maxCoords.x, minCoords.y, maxCoords.z),
			glm::vec3(maxCoords.x, maxCoords.y, maxCoords.z),
			glm::vec3(maxCoords.x, maxCoords.y, maxCoords.z),
			glm::vec3(maxCoords.x, maxCoords.y, minCoords.z),
			glm::vec3(maxCoords.x, maxCoords.y, minCoords.z),
			glm::vec3(maxCoords.x, minCoords.y, minCoords.z),            // RIGHT BOX

		};

		// Add the bounding box vertices to the mesh's vertices
		for (const auto& vertex : boundingBoxVertices)
		{
			Vertex meshVertex;
			meshVertex.position = vertex;
			bVertices.push_back(meshVertex);
		}
		// Create and initialize the buffers
		initBuffers();

		boundingbox();


		return (mLoaded = true);
	}
	// We shouldn't get here so return failure
	return false;
}


void Mesh::draw()
{
	if (!mLoaded) return;

	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
	//cout << "size " << mVertices.size() << endl;
	//glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	//glDeleteBuffers(1, &mVBO);

	glBindVertexArray(mVAO2);
	glDrawArrays(GL_LINES, 0, bVertices.size());
	//cout << "size " << mVertices.size() << endl;
	//glDisableVertexAttribArray(2);
	glBindVertexArray(0);
	//glDeleteBuffers(1, &mVBO2);
}

void Mesh::drawBoundingBox()
{
	if (!mLoaded) return;

	glBindVertexArray(mVAO2);
	glDrawArrays(GL_LINES, 0, bVertices.size());
	//cout << "size " << mVertices.size() << endl;
	//glDisableVertexAttribArray(2);
	glBindVertexArray(0);
	//glDeleteBuffers(1, &mVBO2);
}


void Mesh::initBuffers()
{
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 5 * sizeof(GLfloat), (GLvoid*)(0));
	glEnableVertexAttribArray(0);

	//Texture Coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);


	//glBindVertexArray(0);
}

void Mesh::boundingbox()
{
	glGenVertexArrays(1, &mVAO2);
	glBindVertexArray(mVAO2);

	glGenBuffers(1, &mVBO2);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO2);
	glBufferData(GL_ARRAY_BUFFER, bVertices.size() * sizeof(Vertex), &bVertices[0], GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 5 * sizeof(GLfloat), (GLvoid*)(0));
	glEnableVertexAttribArray(0);


	//glBindVertexArray(2);

}
