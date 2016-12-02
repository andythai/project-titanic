#include "Bound.h"
#include "../Window.h"

Bound::Bound()
{
	collision = false;
	toWorld = glm::mat4(1.0f);

	// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(VAO);

	// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
	// you want to draw, such as vertices, normals, colors, etc.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);

	// Fill axis lists
	update();
}

Bound::~Bound()
{
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Bound::draw(GLuint shaderProgram)
{
	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * toWorld;
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	uCollision = glGetUniformLocation(shaderProgram, "collision_color");
	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);

	if (collision == true) {
		glUniform3f(uCollision, 1.0f, 0.0f, 0.0f);
	}
	else {
		glUniform3f(uCollision, 0.0f, 1.0f, 0.0f);
	}

	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

bool Bound::check_collision(Bound * other) {
	std::sort(this->x_list.begin(), this->x_list.end());
	std::sort(this->y_list.begin(), this->y_list.end());
	std::sort(this->z_list.begin(), this->z_list.end());

	std::sort(other->x_list.begin(), other->x_list.end());
	std::sort(other->y_list.begin(), other->y_list.end());
	std::sort(other->z_list.begin(), other->z_list.end());
	
	GLfloat source_x_min = x_list[0];
	GLfloat source_x_max = x_list[7];
	GLfloat source_y_min = y_list[0];
	GLfloat source_y_max = y_list[7];
	GLfloat source_z_min = z_list[0];
	GLfloat source_z_max = z_list[7];

	GLfloat target_x_min = other->x_list[0];
	GLfloat target_x_max = other->x_list[7];
	GLfloat target_y_min = other->y_list[0];
	GLfloat target_y_max = other->y_list[7];
	GLfloat target_z_min = other->z_list[0];
	GLfloat target_z_max = other->z_list[7];

	bool x_collides = false;
	bool y_collides = false;
	bool z_collides = false;

	// Check x-axis overlaps
	if (source_x_min >= target_x_min && source_x_min <= target_x_max
		|| source_x_max >= target_x_min && source_x_max <= target_x_max) {
		x_collides = true;
	}

	// Check y-axis overlaps
	if (source_y_min >= target_y_min && source_y_min <= target_y_max
		|| source_y_max >= target_y_min && source_y_max <= target_y_max) {
		y_collides = true;
	}

	// Check z-axis overlaps
	if (source_z_min >= target_z_min && source_z_min <= target_z_max
		|| source_z_max >= target_z_min && source_z_max <= target_z_max) {
		z_collides = true;
	}

	// Checks for collision
	if (x_collides && y_collides & z_collides) {
		this->collision = true;
		other->collision = true;
		return true;
	}
	else {
		this->collision = false;
		other->collision = false;
		return false;
	}

	// Check x-axis overlaps
	/*
	if (this->x_list[0] >= other->x_list[0] && this->x_list[0] <= other->x_list[1] 
		|| this->x_list[1] >= other->x_list[0] && this->x_list[1] <= other->x_list[1]) {
		this->collision = true;
		other->collision = true;
		return true;
	}
	else {
		this->collision = false;
		other->collision = false;
		return false;
	}
	*/
}

void Bound::update()
{
	// Fill axis lists
	x_list.clear();
	y_list.clear();
	z_list.clear();
	for (int i = 0; i < 8; i++) {
		glm::vec4 new_point = glm::vec4(vertices[i][0], vertices[i][1], vertices[i][2], 1.0f);
		new_point = this->toWorld * new_point;
		x_list.push_back(new_point[0]);
		y_list.push_back(new_point[1]);
		z_list.push_back(new_point[2]);
	}
	std::sort(this->x_list.begin(), this->x_list.end());
	std::sort(this->y_list.begin(), this->y_list.end());
	std::sort(this->z_list.begin(), this->z_list.end());
}

void Bound::spin(float deg)
{
	// If you haven't figured it out from the last project, this is how you fix spin's behavior
	toWorld = toWorld * glm::rotate(glm::mat4(1.0f), 1.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
}

