/****************************/
/*****	by ANDY THAI	*****/
/****************************/

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include "Cube.h"
#include "shader.h"

// Include
#include "GLFWStarterProject/Skybox.h"
#include "GLFWStarterProject/OBJObject.h"
#include "GLFWStarterProject/Audio.h"
#include "GLFWStarterProject/Bound.h"
#include "GLFWStarterProject/Terrain.h"
#include "GLFWStarterProject/Water.h"
#include "GLFWStarterProject/Island.h"
#include "GLFWStarterProject/Particles.h"
#include <time.h>

class Window
{
public:
	static int width;
	static int height;
	static glm::mat4 P; // P for projection
	static glm::mat4 V; // V for view
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void regen();
	static void regen_single();

	// Used for camera controls
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static glm::vec3 trackBallMapping(glm::dvec2 curPos);
	static void cursor_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};

#endif
