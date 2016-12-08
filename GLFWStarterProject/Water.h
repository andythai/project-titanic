#ifndef _WATER_H_
#define _WATER_H_


#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Water {
public:
	//Constructors
	Water();
	~Water();

	glm::mat4 toWorld;
	glm::vec4 reflectionPlane = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 refractionPlane = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);

	float planeLength;

	void draw(GLuint shaderprogram);
	void scale(float xoff, float yoff, float zoff);
	GLuint loadCubemap();
	unsigned char* Water::loadPPM(const char* filename, int& width, int& height);
	GLuint loadDuDv();
	GLuint loadWaterNormal();
	//vertices
	GLfloat vertices[6][3] = {
		-1.0f,  0.0f, -1.0f,
		-1.0f,  0.0f,  1.0f,
		1.0f,  0.0f, 1.0f,
		-1.0f,  0.0f,  -1.0f,
		1.0f,  0.0f, 1.0f,
		1.0f,  0.0f,  -1.0f
	};

	// These variables are needed for the shader program
	GLuint VAO, VBO, ReflectionFBO, RefractionFBO;
	GLuint uProjection, uModelview, uModel;
	GLuint reflectionTextureID, depthBuffer;
	GLuint refractionTextureID, refractionDepthTextureID;
	GLuint cubemapTexture, dudvTexture, waterNormalTexture;
};

#endif