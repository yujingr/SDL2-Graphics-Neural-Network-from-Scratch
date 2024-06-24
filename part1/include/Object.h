#ifndef OBJECT_H
#define OBJECT_H

#include <glad/glad.h>
#include <vector>

#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Shader.h"
#include "Buffer.h"
#include "Transform.h"

class Object {
public:
	// Object Constructor: color, coordinates
	Object(glm::vec3 color, glm::vec3 coord);
	Object(glm::vec3 coord);

	// Object destructor
	~Object();

	// Color rgb
	glm::vec3 rgb = glm::vec3(1.0f, 1.0f, 1.0f);

	// Coordinates
	glm::vec3 coordinates = glm::vec3(0.0f, 0.0f, 0.0f);

	// Initialization routine
	void init();
	void update(unsigned int screenWidth, unsigned int screenHeight);
	void render();

	// Transformations
	Transform& getTransform();

protected:
	// Vertices and Indices for Buffer
	std::vector<float> verts;
	std::vector<unsigned int> indices;
	// Buffer
	Buffer myBuffer;
	// Shader
	Shader myShader;
	// Transformations
	Transform transform;
	// Projection Matrix
	glm::mat4 projectionMatrix;

};

#endif
