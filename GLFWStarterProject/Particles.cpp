#define _CRT_SECURE_NO_WARNINGS

#include "Particles.h"
#include "../Window.h"
#include <iostream>

Particles::Particles() {
	toWorld = glm::mat4(1.0f);

	// Set up mesh and attribute properties
	GLfloat particle_quad[] = {
		0.0f, 3.0f, 0.0f,
		3.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,

		0.0f, 3.0f, 0.0f,
		3.0f, 3.0f, 0.0f,
		3.0f, 0.0f, 0.0f,
	};
	GLfloat particle_texCoord[] = {
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &TBO);
	glBindVertexArray(VAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_texCoord), particle_texCoord, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Create this->amount default particle instances
	for (GLuint i = 0; i < maxParticles; ++i)
		this->particles.push_back(Particle());
	//std::cout << "Particles size: " << particles.size() << std::endl;
	textureID = loadTexture();

}

Particles::Particles(bool blue) {
	toWorld = glm::mat4(1.0f);

	// Set up mesh and attribute properties
	GLfloat particle_quad[] = {
		0.0f, 1.5f, 0.0f,
		1.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,

		0.0f, 1.5f, 0.0f,
		1.5f, 1.5f, 0.0f,
		1.5f, 0.0f, 0.0f,
	};
	GLfloat particle_texCoord[] = {
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &TBO);
	glBindVertexArray(VAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_texCoord), particle_texCoord, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Create this->amount default particle instances
	for (GLuint i = 0; i < maxParticles; ++i)
		this->particles.push_back(Particle());
	//std::cout << "Particles size: " << particles.size() << std::endl;
	textureID = loadTexture_blue();

}

Particles::~Particles() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &TBO);
}

void Particles::update(GLfloat dt, GLuint newParticles, glm::vec3 offset)
{
	// Add new particles 
	for (GLuint i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle], offset);
	}
	// Update all particles
	for (GLuint i = 0; i < maxParticles; ++i)
	{
		Particle &p = this->particles[i];
		p.life -= dt; // reduce life
		if (p.life > 0.0f)
		{	// particle is alive, thus update
			p.pos += glm::vec3(p.speed.x*dt, p.speed.y*dt - 0.5f * 20 * dt*dt, p.speed.z*dt);
			p.color.a -= dt * 2.5;
		}
	}
}

void Particles::draw(GLuint shaderProgram) {
	glm::mat4 modelview = Window::V * toWorld;
	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glUseProgram(shaderProgram);
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	uModel = glGetUniformLocation(shaderProgram, "model");
	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);
	for (Particle particle : this->particles)
	{
		if (particle.life > 0.0f)
		{
			glUniform3f(glGetUniformLocation(shaderProgram, "offset"), particle.pos.x, particle.pos.y, particle.pos.z);
			glUniform4f(glGetUniformLocation(shaderProgram, "color"), particle.color.x, particle.color.y, particle.color.z, particle.color.w);
			glUniform1i(glGetUniformLocation(shaderProgram, "sprite"), 0);
			glBindVertexArray(VAO);

			//bind texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureID);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBindVertexArray(0);
		}
	}
	// Don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GLuint Particles::loadTexture() {
	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_2D, textureID);

	// Make sure no bytes are padded:
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Select GL_MODULATE to mix texture with polygon color for shading:
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Load front
	image = loadPPM("assets/particles/halfhalf.ppm", width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	// Sets texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Unbinds texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

GLuint Particles::loadTexture_blue() {
	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_2D, textureID);

	// Make sure no bytes are padded:
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Select GL_MODULATE to mix texture with polygon color for shading:
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Load front
	image = loadPPM("assets/particles/halfhalf_blue.ppm", width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	// Sets texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Unbinds texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

// Stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;
GLuint Particles::firstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < maxParticles; ++i) {
		if (this->particles[i].life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (GLuint i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void Particles::respawnParticle(Particle &particle, glm::vec3 offset)
{
	GLfloat randomx = ((rand() % 100) - 50) / 10.0f;
	GLfloat randomy = ((rand() % 100) - 50) / 10.0f;
	GLfloat randomz = ((rand() % 100) - 50) / 10.0f;
	GLfloat randomSpeedx = ((rand() % 100) - 50) / 100.0f;
	GLfloat randomSpeedy = ((rand() % 100) - 50) / 100.0f;
	GLfloat randomSpeedz = ((rand() % 100) - 50) / 100.0f;

	GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
	particle.pos = glm::vec3(randomx / 3.0f + offset.x, offset.y, offset.z + randomz / 3.0f);
	particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.life = 30.0f;
	particle.speed = glm::vec3(0.2f + randomSpeedx, 1.0f + randomSpeedy, 0.2f + randomSpeedz);
}

/** Load a ppm file from disk.
@input filename The location of the PPM file.  If the file is not found, an error message
will be printed and this function will return 0
@input width This will be modified to contain the width of the loaded image, or 0 if file not found
@input height This will be modified to contain the height of the loaded image, or 0 if file not found
@return Returns the RGB pixel data as interleaved unsigned chars (R0 G0 B0 R1 G1 B1 R2 G2 B2 .... etc) or 0 if an error ocured
**/
unsigned char* Particles::loadPPM(const char* filename, int& width, int& height)
{
	const int BUFSIZE = 128;
	FILE* fp;
	unsigned int read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
		width = 0;
		height = 0;
		return NULL;
	}

	// Read magic number:
	retval_fgets = fgets(buf[0], BUFSIZE, fp);

	// Read width and height:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
	width = atoi(buf[1]);
	height = atoi(buf[2]);

	// Read maxval:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');

	// Read image data:
	rawData = new unsigned char[width * height * 3];
	read = fread(rawData, width * height * 3, 1, fp);
	fclose(fp);
	if (read != 1)
	{
		std::cerr << "error parsing ppm file, incomplete data" << std::endl;
		delete[] rawData;
		width = 0;
		height = 0;
		return NULL;
	}

	return rawData;
}