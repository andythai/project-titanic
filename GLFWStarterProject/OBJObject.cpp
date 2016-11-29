#define _CRT_SECURE_NO_DEPRECATE
#include "OBJObject.h"
#include "../Window.h"


OBJObject::OBJObject()
{}

OBJObject::OBJObject(const char* filepath)
{
	parse(filepath);
	initialize();
}

OBJObject::~OBJObject()
{
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void OBJObject::initialize()
{
	toWorld = glm::mat4(1.0f);

	// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO); // Stores large number of vertices
	glGenBuffers(1, &NBO);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(VAO);

	// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
	// you want to draw, such as vertices, normals, colors, etc.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

					 // We've sent the vertex data over to OpenGL, but there's still something missing.
					 // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.

					 /* NBO */

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

	// Enable the usage of layout location 1 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(1,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

					 // We've sent the vertex data over to OpenGL, but there's still something missing.
					 // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.

					 /*END*/


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}

void OBJObject::parse(const char *filepath)
{
	// parse the OBJ file
	// Populate the face indices, vertices, and normals vectors with the OBJ Object data

	std::ifstream objfile;
	objfile.open(filepath, std::ifstream::in);
	if (!objfile) {
		std::cerr << "error loading file" << std::endl;
		exit(-1);
	}


	// Double parsing centering
	GLfloat min_x = FLT_MAX;
	GLfloat max_x = -FLT_MAX;
	GLfloat min_y = FLT_MAX;
	GLfloat max_y = -FLT_MAX;
	GLfloat min_z = FLT_MAX;
	GLfloat max_z = -FLT_MAX;

	std::string line;

	while (std::getline(objfile, line)) {
		std::istringstream linestream(line); // to "tokenize"
		std::string linetype;
		linestream >> linetype;

		if (linetype == "v") {
			float x, y, z;  // vertex coordinates
							// float r,g,b;  // vertex color (not in use for now)
			linestream >> x;
			linestream >> y;
			linestream >> z;
			vertices.push_back(glm::vec3(x, y, z));

			if (x > max_x) {
				max_x = x;
			}
			else if (x < min_x) {
				min_x = x;
			}

			if (y > max_y) {
				max_y = y;
			}
			else if (y < min_y) {
				min_y = y;
			}

			if (z > max_z) {
				max_z = z;
			}
			else if (z < min_z) {
				min_z = z;
			}
		}
		else if (linetype == "vn") {
			float x, y, z;  // normal vector
			linestream >> x;
			linestream >> y;
			linestream >> z;
			normals.push_back(glm::normalize(glm::vec3(x, y, z)));
		}
		else if (linetype == "f") {
			std::string s1, s2, s3;
			unsigned int index1, index2, index3;
			linestream >> s1;
			linestream >> s2;
			linestream >> s3;
			index1 = std::stoi(s1, nullptr) - 1; // indexing in .obj files start at 1
			index2 = std::stoi(s2, nullptr) - 1;
			index3 = std::stoi(s3, nullptr) - 1;
			indices.push_back(index1);
			indices.push_back(index2);
			indices.push_back(index3);
		}
	}

	GLfloat avg_x = (max_x - min_x) / 2;
	GLfloat avg_y = (max_y - min_y) / 2;
	GLfloat avg_z = (max_z - min_z) / 2;
	GLfloat size;

	if (max_x - min_x > max_y - min_y)
		size = max_x - min_x;
	else {
		size = max_y - min_y;
	}
	if (max_z - min_z > size) {
		size = max_z - min_z;
	}

	for (int i = 0; i < vertices.size(); i++) {
		vertices[i][0] = (vertices[i][0] - avg_x) / size;
		vertices[i][1] = (vertices[i][1] - avg_y) / size;
		vertices[i][2] = (vertices[i][2] - avg_z) / size;
	}

	objfile.close();
}

void OBJObject::draw(GLuint shaderProgram)
{
	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * toWorld;
	// We need to calculate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs

	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	uModel = glGetUniformLocation(shaderProgram, "model");
	uView = glGetUniformLocation(shaderProgram, "view");
	uColor = glGetUniformLocation(shaderProgram, "material.ambient");
	uDiffuse = glGetUniformLocation(shaderProgram, "material.diffuse");
	uSpecular = glGetUniformLocation(shaderProgram, "material.specular");
	uShininess = glGetUniformLocation(shaderProgram, "material.shininess");

	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &Window::V[0][0]);

	// Materials
	glUniform3f(uColor, object_color.r, object_color.g, object_color.b);
	glUniform3f(uDiffuse, diffuse.r, diffuse.g, diffuse.b);
	glUniform3f(uSpecular, specular.r, specular.g, specular.b);
	glUniform1f(uShininess, shininess);

	// Now draw the object. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);

	// Tell OpenGL to draw with triangles, using indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void OBJObject::update()
{
	spin(0.1f);
}

void OBJObject::spin(float deg)
{
	// If you haven't figured it out from the last project, this is how you fix spin's behavior
	toWorld = toWorld * glm::rotate(glm::mat4(1.0f), 1.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
}

void OBJObject::translate(float x, float y, float z)
{
	glm::mat4 matrix =
	{ 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1 };

	this->toWorld = matrix * this->toWorld;
}

void OBJObject::origin()
{
	glm::mat4 matrix =
	{ 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 };

	matrix[3][0] = -this->toWorld[3][0];
	matrix[3][1] = -this->toWorld[3][1];
	matrix[3][2] = -this->toWorld[3][2];

	this->toWorld = matrix * this->toWorld;
}

void OBJObject::origin_preserve_z()
{
	glm::mat4 matrix =
	{ 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 };

	matrix[3][0] = -this->toWorld[3][0];
	matrix[3][1] = -this->toWorld[3][1];

	this->toWorld = matrix * this->toWorld;
}

void OBJObject::reset()
{
	this->toWorld = glm::mat4(1.0f);
}

void OBJObject::scale(float mult)
{
	glm::mat4 scale_matrix =
	{ mult, 0, 0, 0,
		0, mult, 0, 0,
		0, 0, mult, 0,
		0, 0, 0, 1 };

	float matX = this->toWorld[3][0];
	float matY = this->toWorld[3][1];
	float matZ = this->toWorld[3][2];

	glm::mat4 to_origin_matrix =
	{ 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-matX, -matY, -matZ, 1 };

	glm::mat4 reset_matrix =
	{ 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		matX, matY, matZ, 1 };

	this->toWorld = to_origin_matrix * this->toWorld;
	this->toWorld = scale_matrix * this->toWorld;
	this->toWorld = reset_matrix * this->toWorld;
}

void OBJObject::orbit(float deg)
{

	float radians = deg / 180.0f * glm::pi<float>();
	glm::mat4 matrix =
	{ cos(radians), sin(radians), 0, 0,
		-sin(radians), cos(radians), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 };

	this->toWorld = matrix * this->toWorld;
}

void OBJObject::rotate(float angle, glm::vec3 axis)
{
	float matX = this->toWorld[3][0];
	float matY = this->toWorld[3][1];
	float matZ = this->toWorld[3][2];

	glm::mat4 to_origin_matrix =
	{ 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-matX, -matY, -matZ, 1 };

	glm::mat4 reset_matrix =
	{ 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		matX, matY, matZ, 1 };

	this->toWorld = to_origin_matrix * this->toWorld;

	this->toWorld = glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), axis) * this->toWorld;

	this->toWorld = reset_matrix * this->toWorld;
}

void OBJObject::setAmbient(float r, float g, float b)
{
	object_color = { r, g, b };
}

void OBJObject::setDiffuse(float r, float g, float b)
{
	diffuse = { r, g, b };
}

void OBJObject::setSpecular(float r, float g, float b)
{
	specular = { r, g, b };
}

void OBJObject::setShininess(int number)
{
	if (number > 128) {
		shininess = 128;
	}
	else if (number < 0) {
		shininess = 0;
	}
	else {
		shininess = number;
	}
}