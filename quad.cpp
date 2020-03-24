#include "quad.h"
#include <glad/glad.h>
#include <cstdio>

Quad::Quad()
{
	std::vector<GLfloat> points = {
		// top-left
		-1.0f, 1.0f, 0.0f,
		// top-right
		1.0f, 1.0f, 0.0f,
		// bottom right
		1.0f, -1.0f, 0.0f,
		// bottom left
		-1.0f, -1.0f, 0.0f
	};

	std::vector<GLfloat> normals = {
		// top-left
		0.0f, 0.0f, 1.0f,
		// top-right
		0.0f, 0.0f, 1.0f,
		// bottom right
		0.0f, 0.0f, 1.0f,
		// bottom left
		0.0f, 0.0f, 1.0f
	};

	std::vector<GLfloat> texCoords = {
		// top-left
		0.0f, 1.0f,
		// top-right
		1.0f, 1.0f,
		// bottom right
		1.0f, 0.0f,
		// bottom left
		0.0f, 0.0f
	};

	std::vector<GLuint> elementIndices = {
		0, 1, 3,
		1, 2, 3
	};

	initBuffers(&elementIndices, &points, &normals, &texCoords);
}
