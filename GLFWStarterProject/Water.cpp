#define _CRT_SECURE_NO_WARNINGS
#include "Water.h"
#include "../Window.h"
#include <iostream>

using namespace std;

Water::Water() {
	toWorld = glm::mat4(1.0f);
	planeLength = 2.0f;

	// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenFramebuffers(1, &ReflectionFBO);
	glGenFramebuffers(1, &RefractionFBO);

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

					 // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);


	////TODO Frame Buffer
	////Reflection Frame buffer
	//glBindFramebuffer(GL_FRAMEBUFFER, ReflectionFBO);
	//glDrawBuffer(GL_COLOR_ATTACHMENT0); 

	////create reflection texture to frame buffer
	//glGenTextures(1, &reflectionTextureID);
	//glBindTexture(GL_TEXTURE_2D, reflectionTextureID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::width, Window::height, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLuint)0);
	//// Set bi-linear filtering for both minification and magnification
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//
	////attach current texture to the frame buffer
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, reflectionTextureID, (GLint)0);

	////create depth buffer(render buffer)
	//glGenRenderbuffers(1, &depthBuffer);
	//glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Window::width, Window::height);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	//
	////unbind frame buffer
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glViewport(0, 0, Window::width, Window::height);


	////Same with Refraction FBO
	////Reflection Frame buffer
	//glBindFramebuffer(GL_FRAMEBUFFER, RefractionFBO);
	//glDrawBuffer(GL_COLOR_ATTACHMENT0);

	////create reflection texture to frame buffer
	//glGenTextures(1, &refractionTextureID);
	//glBindTexture(GL_TEXTURE_2D, refractionTextureID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::width, Window::height, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLuint)0);
	//// Set bi-linear filtering for both minification and magnification
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	////attach current texture to the frame buffer
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, refractionTextureID, (GLint)0);

	////create Depth Buffer Texture Attachment
	//glGenTextures(1, &refractionDepthTextureID);
	//glBindTexture(GL_TEXTURE_2D, refractionDepthTextureID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, Window::width, Window::height, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLuint)0);
	//// Set bi-linear filtering for both minification and magnification
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	////attach current texture to the frame buffer
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, refractionDepthTextureID, (GLint)0);

	////unbind frame buffer
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glViewport(0, 0, Window::width, Window::height);

	//cube map texture
	cubemapTexture = loadCubemap();

	//load water Du/Dv map texture
	dudvTexture = loadDuDv();

	//load water normal map texture
	waterNormalTexture = loadWaterNormal();
}

GLuint Water::loadWaterNormal() {
	glGenTextures(1, &waterNormalTexture);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_2D, waterNormalTexture);

	// Make sure no bytes are padded:
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Select GL_MODULATE to mix texture with polygon color for shading:
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Load front
	image = loadPPM("assets/water/water-texture-2-normal.ppm", width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	// Sets texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Unbinds texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return waterNormalTexture;
}
GLuint Water::loadDuDv() {
	glGenTextures(1, &dudvTexture);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_2D, dudvTexture);

	// Make sure no bytes are padded:
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Select GL_MODULATE to mix texture with polygon color for shading:
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Load front
	image = loadPPM("assets/water/waterDUDV.ppm", width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	// Sets texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Unbinds texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return dudvTexture;
}

Water::~Water() {
	// Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a 
	// large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteFramebuffers(1, &ReflectionFBO);
	glDeleteTextures(1, &reflectionTextureID);
	glDeleteRenderbuffers(1, &depthBuffer);
}

void Water::draw(GLuint shaderProgram)
{
	float pi = glm::pi<float>();
	// Calculate the combination of the model and view (camera inverse) matrices
	glm::mat4 modelview = Window::V * toWorld;
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

	//send reflection/refraction plane info to vertex shader
	glUniform4f(glGetUniformLocation(shaderProgram, "reflectPlane"), reflectionPlane.x, reflectionPlane.y, reflectionPlane.z, reflectionPlane.w);

	//amplitude
	glUniform1f(glGetUniformLocation(shaderProgram, "amplitudes[0]"), 0.5f / (0.0 + 10.0));
	glUniform1f(glGetUniformLocation(shaderProgram, "amplitudes[1]"), 0.5f / (1.0 + 10.0));
	glUniform1f(glGetUniformLocation(shaderProgram, "amplitudes[2]"), 0.5f / (2.0 + 10.0));
	glUniform1f(glGetUniformLocation(shaderProgram, "amplitudes[3]"), 0.5f / (3.0 + 10.0));

	//wavelength
	glUniform1f(glGetUniformLocation(shaderProgram, "waveLengths[0]"), 8.0f * pi / (0.0 + 10.0));
	glUniform1f(glGetUniformLocation(shaderProgram, "waveLengths[1]"), 8.0f * pi / (1.0 + 10.0));
	glUniform1f(glGetUniformLocation(shaderProgram, "waveLengths[2]"), 8.0f * pi / (2.0 + 10.0));
	glUniform1f(glGetUniformLocation(shaderProgram, "waveLengths[3]"), 8.0f * pi / (3.0 + 10.0));

	//speed
	glUniform1f(glGetUniformLocation(shaderProgram, "speeds[0]"), 1.0f + 2 * 0.0f);
	glUniform1f(glGetUniformLocation(shaderProgram, "speeds[1]"), 1.0f + 2 * 1.0f);
	glUniform1f(glGetUniformLocation(shaderProgram, "speeds[2]"), 1.0f + 2 * 2.0f);
	glUniform1f(glGetUniformLocation(shaderProgram, "speeds[3]"), 1.0f + 2 * 3.0f);

	//direction
	glUniform2f(glGetUniformLocation(shaderProgram, "wave_direction[0]"), cos(pi / 5.0f), sin(pi / 2.0f));
	glUniform2f(glGetUniformLocation(shaderProgram, "wave_direction[1]"), cos(pi / 9.0f), sin(pi / 9.0f));
	glUniform2f(glGetUniformLocation(shaderProgram, "wave_direction[2]"), cos(-pi / 7.5f), sin(-pi / 7.5f));
	glUniform2f(glGetUniformLocation(shaderProgram, "wave_direction[3]"), cos(-pi / 25.4f), sin(-pi / 25.4f));

	//planeLength
	glUniform1f(glGetUniformLocation(shaderProgram, "u_waterPlaneLength"), planeLength);

	//cubemap
	glUniform1i(glGetUniformLocation(shaderProgram, "skybox"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "dudv"), 1);
	glUniform1i(glGetUniformLocation(shaderProgram, "normalMap"), 2);

	//light direction
	glUniform3f(glGetUniformLocation(shaderProgram, "LightDirection"), -0.0459845f, 0.0925645f, 0.994644f);

	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);

	//bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, dudvTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, waterNormalTexture);

	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawArrays(GL_TRIANGLES, 0, 6);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);

}

void Water::scale(float xoff, float yoff, float zoff) {
	glm::mat4 scaleM = glm::scale(glm::mat4(1.0f), glm::vec3(xoff, yoff, zoff));
	toWorld = toWorld * scaleM;

	planeLength *= xoff;
}

GLuint Water::loadCubemap() {

	glGenTextures(1, &cubemapTexture);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	// Make sure no bytes are padded:
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Select GL_MODULATE to mix texture with polygon color for shading:
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Load front
	image = loadPPM("assets/skybox/front.ppm", width, height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	// Load back
	image = loadPPM("assets/skybox/back.ppm", width, height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	// Load bottom
	image = loadPPM("assets/skybox/bottom.ppm", width, height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	// Load left
	image = loadPPM("assets/skybox/left.ppm", width, height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	// Load right
	image = loadPPM("assets/skybox/right.ppm", width, height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	// Load top
	image = loadPPM("assets/skybox/top.ppm", width, height);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	// Sets texture parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Unbinds texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return cubemapTexture;
}

/** Load a ppm file from disk.
@input filename The location of the PPM file.  If the file is not found, an error message
will be printed and this function will return 0
@input width This will be modified to contain the width of the loaded image, or 0 if file not found
@input height This will be modified to contain the height of the loaded image, or 0 if file not found
@return Returns the RGB pixel data as interleaved unsigned chars (R0 G0 B0 R1 G1 B1 R2 G2 B2 .... etc) or 0 if an error ocured
**/
unsigned char* Water::loadPPM(const char* filename, int& width, int& height)
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