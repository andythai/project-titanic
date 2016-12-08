/****************************/
/*****	by ANDY THAI	*****/
/****************************/

#ifndef _OBJOBJECT_H_
#define _OBJOBJECT_H_


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
#include <fstream> // std::ifstream
#include <string> // std::string
#include <sstream> // std::istringstream
#include "Bound.h"
#include "Island.h"

class OBJObject
{
public:
	// Constructors
	OBJObject();
	OBJObject(const char* filepath);
	~OBJObject();

	// toWorld vector
	glm::mat4 toWorld;

	bool toDraw = true;

	// Containers
	std::vector<GLuint> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;

	// Bounding box
	Bound * bounding_box = new Bound(25.0f, 5.0f, 3.0f);

	// Colors
	glm::vec3 object_color = { 0.75f, 0.75f, 0.95f };
	glm::vec3 diffuse = { 0.52f, 0.5f, 0.51f };
	glm::vec3 specular = { 0.52f, 0.5f, 0.5f };

	// For specular
	int shininess = 32;

	void initialize();
	void parse(const char* filepath);
	bool checkBounds(Island*);
	void draw(GLuint);
	void update();
	void spin(float);
	void translate(float x, float y, float z);
	void reset();
	void scale(float mult);
	void orbit(float deg);
	void origin();
	void origin_preserve_z();
	void rotate(float angle, glm::vec3 axis);
	void setAmbient(float r, float g, float b);
	void setDiffuse(float r, float g, float b);
	void setSpecular(float r, float g, float b);
	void setShininess(int number);


	// These variables are needed for the shader program
	GLuint VBO, VAO, EBO, NBO;
	GLuint uProjection, uModelview, uModel, uView, uLight, uColor, uDiffuse, uSpecular, uShininess;
};
#endif