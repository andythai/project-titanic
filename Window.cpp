/****************************/
/*****	by ANDY THAI	*****/
/****************************/

#include "window.h"

const char* window_title = "PROJECT TITANIC";

// INITIALIZATIONS FOR RENDERED OBJECTS
GLint bounds_shaderProgram, skybox_shaderProgram, toon_shaderProgram, 
	terrain_shaderProgram, water_shaderProgram, particle_shaderProgram, outline_shaderProgram;
Skybox * skybox;							// Skybox
OBJObject * model;							// Player's model, ship
OBJObject * buoy;							// Collectible buoy
std::vector<Island*> isles;					// Vector of generated islands
std::vector<OBJObject*> buoys;				// Vector of generated buoys
Particles * explosion;						// Explosion effect on collision with land
Particles * water_trail;					// Water trail effect from ship

// Water variables
Water * ocean;
bool go_backwards = false;					// Determines when to loop wave movements
clock_t tpf;								// Clock to count time elapsed 
float elapsedTime;							// How much time has elapsed
float time2;								// Reverse time counter for wave looping movement

// AUDIO
Audio * sound;

// TOGGLE VARIABLES (Do not edit!)
bool TOGGLE_BOUNDING_BOXES = false;			// Press B to show bounding boxes.
int SHOW_NORMALS = 0;						// Variable to color terrains with their normals
bool FREE_CAMERA = false;					// Press C to allow 'free' control of the camera
bool SHOW_OUTLINES = true;					// Press O to toggle silhouettes
bool SHOW_WATER = true;						// Press L to hide/unhide water
bool DEBUG = false;							// Use 0 to toggle invincibility, disables bounds collision game over condition
bool SPRAY_PARTICLES = false;				// Press F to spray blue particle trails from the ship
int TOON_SHADING = true;					// Press P to toggle discretized color shading

// GAME OPTIONS (You may edit these!)
const int ISLAND_NUM = 10;					// Default: 10
const int BUOY_NUM = 10;					// Default: 10
const float ISLAND_MAX_RANGE = 1000.0f;		// Minimum range to generate islands
const float ISLAND_MIN_RANGE = -1000.0f;	// Maximum range to generate islands

// Non-editable game vars
bool GAME_OVER = false;						// Really self-explanatory
bool PRINT_GAME_OVER = false;				// Helps prints only one game over message
float game_over_rotate = 0.0f;				// Keeps track of ship's rotation post-game over
bool ship_collided = false;					// Helps calculate when ship has collided and when to color bounds red
int key_pressed;							// Holds currently pressed key
bool key_down;								// Determines if any key is currently held down
float ship_angle = 0.0f;					// Store angle of ship to rotate it
glm::vec3 STORED_CAM_POS;					// Store original camera position to recover it for C
float STORED_SHIP_ANGLE;					// Store original ship angle to recover it for C
Island * tutorial_island;					// Used to stop islands from being generated on top of the ship
GLuint normal_toggle_var;					// Sends information to terrain shader for normals coloring		
GLuint toon_toggle_var;						// Sends information to toon shader to toggle toon coloring		
GLuint uCam_pos;							// Sends camera pos information to shader
GLuint uCam_look_at;						// Sends camera look_at information to shader
glm::vec3 lastPoint;						// Holds last point for mouse/cursor movement
bool place_island = true;					// Used to check if two islands are overlapping
bool intersects_origin = false;				// Used to check if island overlaps ship starting area
glm::mat4 ORIGINAL_SHIP_POS;				// Holds ship's initial toWorld
glm::mat4 ORIGINAL_SHIP_BOUND_POS;			// Holds ship's bound's initial toWorld;
int NUM_COLLECTED = 0;						// Number of buoys collected

// SHADERS: On some systems you need to change this to the absolute path
#define BOUNDS_VERTEX_SHADER_PATH "../bounds_shader.vert"
#define BOUNDS_FRAGMENT_SHADER_PATH "../bounds_shader.frag"

#define TERRAIN_VERTEX_SHADER_PATH "../terrainShader.vert"
#define TERRAIN_FRAGMENT_SHADER_PATH "../terrainShader.frag"

#define SKYBOX_VERTEX_SHADER_PATH "../skybox.vert"
#define SKYBOX_FRAGMENT_SHADER_PATH "../skybox.frag"

#define TOON_VERTEX_SHADER_PATH "../toonshader.vert"
#define TOON_FRAGMENT_SHADER_PATH "../toonshader.frag"

#define WATER_VERTEX_SHADER_PATH "../water_shader.vert"
#define WATER_FRAGMENT_SHADER_PATH "../water_shader.frag"

#define PARTICLE_VERTEX_SHADER_PATH "../ParticleShader.vert"
#define PARTICLE_FRAGMENT_SHADER_PATH "../ParticleShader.frag"

#define OUTLINE_VERTEX_SHADER_PATH "../outline_shader.vert"
#define OUTLINE_FRAGMENT_SHADER_PATH "../outline_shader.frag"

// Default camera parameters
glm::vec3 cam_pos(-20.3139f, 47.818f, -116.562f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);				// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);						// up | What orientation "up" is

// Window variables
int Window::width;
int Window::height;

// Matrices
glm::mat4 Window::P;	// Projection
glm::mat4 Window::V;	// Camera matrix


void Window::initialize_objects()
{
	std::cout << "\nStarting Project Titanic..." << std::endl;
	std::cout << "\nInitializing variables..." << std::endl;

	// RNG initializer
	std::srand(std::time(0));
	
	// Skybox initialization
	skybox = new Skybox(1000);
	skybox->loadCubemap();

	// Water initialization
	ocean = new Water();
	ocean->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	ocean->scale(4000.0f, 1.0f, 4000.0f);
	
	// Start clock for water waves
	tpf = clock();

	// Initialize constant island to prevent islands spawning on ship
	std::cout << "\nGenerating " << ISLAND_NUM << " island(s)..." << std::endl;
	tutorial_island = new Island(2, rand() % 3 + 1); // size, color
	tutorial_island->update();


	// Randomly place islands
	for (int i = 0; i < ISLAND_NUM; i++) {
		int terrainColor = rand() % 3 + 1;
		isles.push_back(new Island(rand() % 2 + 2, terrainColor));
		place_island = true;

		// Repeat if island intersects / collides
		while (place_island == true) {
			place_island = false;
			intersects_origin = false;
			float isle_x = (ISLAND_MIN_RANGE + 1) + (((float)rand()) / (float)RAND_MAX) * (ISLAND_MAX_RANGE - (ISLAND_MIN_RANGE + 1));
			float isle_z = (ISLAND_MIN_RANGE + 1) + (((float)rand()) / (float)RAND_MAX) * (ISLAND_MAX_RANGE - (ISLAND_MIN_RANGE + 1));
			std::cout << "\nIsland #" << i + 1 << std::endl << "x: " << isle_x << ", z: " << isle_z << std::endl;
			isles[i]->move(isle_x, isle_z);

			// Don't spawn island on top of ship
			place_island = isles[i]->checkBounds(tutorial_island);
			if (place_island == true) {
				std::cout << "Uh oh! Island #" << i + 1 << " is too close to the ship! Moving it someplace else..." << std::endl;
				isles[i]->move(-isle_x, -isle_z);
				intersects_origin = true;
			}

			// Don't intersect with other islands
			if (intersects_origin == false) {
				for (int j = 0; j < i; j++) {
					place_island = isles[i]->checkBounds(isles[j]);
					if (place_island == true) {
						std::cout << "Woops! Island #" << i + 1 << " is intersecting with Island #" << j + 1 << "! Trying new coordinates..." << std::endl;
						isles[i]->move(-isle_x, -isle_z);
						break;
					}
				}
			}
		}
	}
	std::cout << "\nIsland generation and placement completed!" << std::endl;
	

	// Initialize ship model
	std::cout << "\nParsing models..." << std::endl;
	model = new OBJObject("assets/cruise_ship/Cruiser 2012.obj");
	model->scale(0.1f);
	model->toWorld = glm::rotate(glm::mat4(1.0f), 90.0f * glm::pi<float>() / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * model->toWorld;
	model->bounding_box->toWorld = glm::rotate(glm::mat4(1.0f), 90.0f * glm::pi<float>() / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * model->bounding_box->toWorld;
	model->translate(0.0f, -3.0f, 0.0f);
	model->bounding_box->update();
	ORIGINAL_SHIP_POS = glm::mat4(model->toWorld);
	ORIGINAL_SHIP_BOUND_POS = glm::mat4(model->bounding_box->toWorld);

	// Buoys
	buoy = new OBJObject("assets/buoy/HarborBuoy.obj");
	buoy->object_color = glm::vec3(1.0f, 165.0f / 255.0f, 0.0f);
	buoy->scale(0.2f);
	buoy->bounding_box->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 3.0f, 1.1f));
	for (int i = 0; i < BUOY_NUM; i++) {
		buoys.push_back(new OBJObject());
		buoys[i]->vertices = buoy->vertices;
		buoys[i]->indices = buoy->indices;
		buoys[i]->normals = buoy->normals;
		buoys[i]->initialize();
		buoys[i]->object_color = buoy->object_color;
		buoys[i]->scale(0.2f);
		buoys[i]->bounding_box->toWorld = buoy->bounding_box->toWorld;
	}
	std::cout << "Models parsed!" << std::endl;
	
	// Randomly place buoys
	std::cout << "\nPlacing buoys..." << std::endl;
	for (int i = 0; i < BUOY_NUM; i++) {
		place_island = true;

		// Repeat if island intersects / collides
		while (place_island == true) {
			place_island = false;
			float isle_x = (ISLAND_MIN_RANGE + 1) + (((float)rand()) / (float)RAND_MAX) * (ISLAND_MAX_RANGE - (ISLAND_MIN_RANGE + 1));
			float isle_z = (ISLAND_MIN_RANGE + 1) + (((float)rand()) / (float)RAND_MAX) * (ISLAND_MAX_RANGE - (ISLAND_MIN_RANGE + 1));
			std::cout << "\nBuoy #" << i + 1 << std::endl << "x: " << isle_x << ", z: " << isle_z << std::endl;
			buoys[i]->translate(isle_x, 0.0f, isle_z);

			// Don't intersect with other islands
			if (intersects_origin == false) {
				for (int j = 0; j < isles.size(); j++) {
					place_island = buoys[i]->checkBounds(isles[j]);
					if (place_island == true) {
						std::cout << "Woops! Buoy #" << i + 1 << " is intersecting with Island #" << j + 1 << "! Trying new coordinates..." << std::endl;
						buoys[i]->translate(-isle_x, 0.0f, -isle_z);
						break;
					}
				}
			}
		}
	}
	std::cout << "\nIsland generation and placement completed!" << std::endl;


	// Initialize particle explosion
	explosion = new Particles();
	water_trail = new Particles(true);

	// Initialize sound files
	std::cout << "\nLoading sound files..." << std::endl;
	std::vector<std::string> sound_files;
	sound_files.push_back("assets/sounds/ocean_waves.wav");
	sound_files.push_back("assets/sounds/ship_bell.wav");
	sound_files.push_back("assets/sounds/crash.wav");
	sound_files.push_back("assets/sounds/cartoon.wav"); // Cartoon by Nicolai Heidlas
	sound_files.push_back("assets/sounds/fanfare.wav");
	sound = new Audio(sound_files);
	std::cout << "Sound files loaded!" << std::endl;

	// Load the shader programs. Make sure you have the correct filepath up top
	std::cout << "\nLoading SHADER files:" << std::endl;
	terrain_shaderProgram = LoadShaders(TERRAIN_VERTEX_SHADER_PATH, TERRAIN_FRAGMENT_SHADER_PATH);
	bounds_shaderProgram = LoadShaders(BOUNDS_VERTEX_SHADER_PATH, BOUNDS_FRAGMENT_SHADER_PATH);
	skybox_shaderProgram = LoadShaders(SKYBOX_VERTEX_SHADER_PATH, SKYBOX_FRAGMENT_SHADER_PATH);
	toon_shaderProgram = LoadShaders(TOON_VERTEX_SHADER_PATH, TOON_FRAGMENT_SHADER_PATH);
	water_shaderProgram = LoadShaders(WATER_VERTEX_SHADER_PATH, WATER_FRAGMENT_SHADER_PATH);
	particle_shaderProgram = LoadShaders(PARTICLE_VERTEX_SHADER_PATH, PARTICLE_FRAGMENT_SHADER_PATH);
	outline_shaderProgram = LoadShaders(OUTLINE_VERTEX_SHADER_PATH, OUTLINE_FRAGMENT_SHADER_PATH);
	std::cout << "\n\nInitialization process complete!" << std::endl;

	// Print controls
	std::cout << "\n\n\nCONTROLS:" << std::endl << std::endl;
	std::cout << "    W - Move ship forward" << std::endl;
	std::cout << "    A - Rotate ship left" << std::endl;
	std::cout << "    S - Move ship backward" << std::endl;
	std::cout << "    D - Rotate ship right" << std::endl;
	std::cout << "    C - Toggle 'free' camera movement" << std::endl;
	std::cout << "        In free camera movement, you can use the mouse wheel to zoom in and out" << std::endl;
	std::cout << "        You can also hold down the mouse and drag it to rotate the camera" << std::endl;
	std::cout << "        WASD in this mode will move the camera instead of the ship" << std::endl;
	std::cout << "    T - Re-initialize terrains" << std::endl;
	std::cout << "    N - Toggle normal colorings for terrain" << std::endl;
	std::cout << "    R - Restart the game on the same map" << std::endl;
	std::cout << "    O - Toggle silhouettes for toon shading" << std::endl;
	std::cout << "    0 - Toggle DEBUG mode - no game overs!" << std::endl;
	std::cout << "    L - Hide/unhide water" << std::endl;
	std::cout << "    M - Regenerate map" << std::endl;
	std::cout << "    , - Regenerate map into singular islands" << std::endl;
	std::cout << "    G - Mute sound" << std::endl;
	std::cout << "    P - Toggle toon coloring" << std::endl;
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(skybox);

	delete(model);

	delete(sound);

	delete(tutorial_island);

	delete(ocean);

	delete(buoy);

	delete(explosion);
	delete(water_trail);

	// Deletes islands and associated terrains
	for (int i = 0; i < isles.size(); i++) {
		delete(isles[i]);
	}

	for (int i = 0; i < buoys.size(); i++) {
		delete(buoys[i]);
	}

	// Delete shaders
	glDeleteProgram(bounds_shaderProgram);
	glDeleteProgram(skybox_shaderProgram);
	glDeleteProgram(toon_shaderProgram);
	glDeleteProgram(outline_shaderProgram);
	glDeleteProgram(terrain_shaderProgram);
	glDeleteProgram(water_shaderProgram);
	glDeleteProgram(particle_shaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 4000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	// This is called when the ship collides
	if (GAME_OVER && PRINT_GAME_OVER) {
		FREE_CAMERA = true; // Take away control from player
		explosion->update(0.1f, 1000, glm::vec3(model->bounding_box->toWorld[3][0], 
			model->bounding_box->toWorld[3][1], model->bounding_box->toWorld[3][2]));
		// Sinking animation
		if (game_over_rotate < 15.0f) {
			model->rotate(0.2f, glm::vec3(0.0f, 0.0f, 1.0f));
			game_over_rotate += 0.2f;
		}
		model->translate(0.0f, -0.015f, 0.0f);
		return;
	}

	// Draw trail for spray particles action (F)
	water_trail->update(0.1f, 50, glm::vec3(model->bounding_box->toWorld[3][0],
		model->bounding_box->toWorld[3][1], model->bounding_box->toWorld[3][2]));

	// Updates camera
	float ship_x = model->bounding_box->toWorld[3][0];
	float ship_y = model->bounding_box->toWorld[3][1];
	float ship_z = model->bounding_box->toWorld[3][2];

	// Play ambient sound
	sound->play(0); // Ocean
	sound->play(3); // Background music

	if (!FREE_CAMERA) {
		cam_look_at = glm::vec3(ship_x, ship_y, ship_z);
	}
	V = glm::lookAt(cam_pos, cam_look_at, cam_up);

	// Ship controls
	if (key_down) {
		if (key_pressed == GLFW_KEY_W) {
			glm::vec3 toMove = glm::vec3(0.0f, 0.0f, 0.55f); // 0.55f for non-20, 2.5x with 20+ islands
			if (ISLAND_NUM >= 20) {
				toMove.z *= 2.5f;
			}
			else if (ISLAND_NUM >= 10) {
				toMove.z *= 1.75f;
			}
			toMove = glm::vec3(glm::rotate(glm::mat4(1.0f), ship_angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(toMove, 1.0f));
			if (!FREE_CAMERA) {
				model->translate(toMove.x, toMove.y, toMove.z);
			}
			cam_pos += toMove;
		}
		else if (key_pressed == GLFW_KEY_S) {
			glm::vec3 toMove = glm::vec3(0.0f, 0.0f, -0.55f); // -0.55 for non-20 islands, 2.5x with 20+ islands
			if (ISLAND_NUM >= 20) {
				toMove.z *= 2.5f;
			}
			else if (ISLAND_NUM >= 10) {
				toMove.z *= 1.75f;
			}
			toMove = glm::vec3(glm::rotate(glm::mat4(1.0f), ship_angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(toMove, 1.0f));
			if (!FREE_CAMERA) {
				model->translate(toMove.x, toMove.y, toMove.z);
			}
			cam_pos += toMove;
		}
		else if (key_pressed == GLFW_KEY_A) {
			float curr_x = model->toWorld[3][0];
			float curr_y = model->toWorld[3][1];
			float curr_z = model->toWorld[3][2];

			if (!FREE_CAMERA) {
				model->translate(-curr_x, -curr_y, -curr_z);
			}
			cam_pos -= glm::vec3(curr_x, curr_y, curr_z);

			float rotation_a = 0.01f;
			if (!FREE_CAMERA) {
				model->toWorld = glm::rotate(glm::mat4(1.0f), rotation_a, glm::vec3(0.0f, 1.0f, 0.0f)) * model->toWorld;
				model->bounding_box->toWorld = glm::rotate(glm::mat4(1.0f), rotation_a, glm::vec3(0.0f, 1.0f, 0.0f)) * model->bounding_box->toWorld;
				model->bounding_box->update();
				model->translate(curr_x, curr_y, curr_z);
			}

			ship_angle += rotation_a;

			cam_pos = glm::vec3(glm::rotate(glm::mat4(1.0f), rotation_a, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cam_pos, 1.0f));
			cam_pos += glm::vec3(curr_x, curr_y, curr_z);
		}
		else if (key_pressed == GLFW_KEY_D) {
			float curr_x = model->toWorld[3][0];
			float curr_y = model->toWorld[3][1];
			float curr_z = model->toWorld[3][2];

			model->translate(-curr_x, -curr_y, -curr_z);
			cam_pos -= glm::vec3(curr_x, curr_y, curr_z);

			float rotation_d = -0.01f;

			ship_angle += rotation_d;

			if (!FREE_CAMERA) {
				model->toWorld = glm::rotate(glm::mat4(1.0f), rotation_d, glm::vec3(0.0f, 1.0f, 0.0f)) * model->toWorld;
				model->bounding_box->toWorld = glm::rotate(glm::mat4(1.0f), rotation_d, glm::vec3(0.0f, 1.0f, 0.0f)) * model->bounding_box->toWorld;
				model->bounding_box->update();
				model->translate(curr_x, curr_y, curr_z);
			}

			cam_pos = glm::vec3(glm::rotate(glm::mat4(1.0f), rotation_d, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cam_pos, 1.0f));
			cam_pos += glm::vec3(curr_x, curr_y, curr_z);
		}
	}

	// CHECKS FOR SHIP COLLISION WITH ISLANDS
	bool land_collides = false;
	for (int i = 0; i < isles.size(); i++) {
		land_collides = model->checkBounds(isles[i]);
		if (land_collides) {
			if (!DEBUG) {
				GAME_OVER = true;
			}
			ship_collided = true;
		}
	}

	// CHECKS FOR SHIP COLLISION WITH BUOYS
	for (int i = 0; i < buoys.size(); i++) {
		if (model->bounding_box->check_collision(buoys[i]->bounding_box) == true && buoys[i]->toDraw == true) {
			buoys[i]->toDraw = false;
			NUM_COLLECTED++;
			if (NUM_COLLECTED == BUOY_NUM) {
				sound->play(4); // fanfare
			}
			else {
				sound->play(1); // ship bell;
			}
			break;
		}
	}

	// COLORS BOUNDING BOXES RED IF COLLIDED
	if (ship_collided) {
		model->bounding_box->collision = true;
		ship_collided = false;
	}


	if (GAME_OVER && !PRINT_GAME_OVER) {
		PRINT_GAME_OVER = true;
		sound->play(2); // Ship crash sound
		std::cout << "\n\nYOU'VE HIT LAND. GAME OVER!" << std::endl;
		FREE_CAMERA = true;
	}
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	/********** RENDER MODEL **********/
	glUseProgram(toon_shaderProgram);
	toon_toggle_var = glGetUniformLocation(toon_shaderProgram, "toggle_toon");
	glUniform1i(toon_toggle_var, TOON_SHADING);
	skybox->sendLight(toon_shaderProgram); // Send light information to toon shader
	model->draw(toon_shaderProgram);
	//buoy->draw(toon_shaderProgram);
	for (int i = 0; i < buoys.size(); i++) {
		if (buoys[i]->toDraw)
			buoys[i]->draw(toon_shaderProgram);
	}
	// Draw outlines
	if (SHOW_OUTLINES) {
		glUseProgram(outline_shaderProgram);
		glDepthMask(GL_TRUE);
		model->draw(outline_shaderProgram);
		//buoy->draw(outline_shaderProgram);
		for (int i = 0; i < buoys.size(); i++) {
			if (buoys[i]->toDraw)
				buoys[i]->draw(outline_shaderProgram);
		}
	}
	/********** MODEL END **********/


	/********** RENDER PARTICLES **********/
	// Explosion on game over
	if (GAME_OVER && PRINT_GAME_OVER) {
		glUseProgram(particle_shaderProgram);
		explosion->draw(particle_shaderProgram);
	}
	if (SPRAY_PARTICLES) {
		glUseProgram(particle_shaderProgram);
		water_trail->draw(particle_shaderProgram);
	}
	/********** END PARTICLES **********/


	/********** RENDER TERRAIN **********/
	glUseProgram(terrain_shaderProgram);
	// Send uniforms
	normal_toggle_var = glGetUniformLocation(terrain_shaderProgram, "normal_toggle");
	glUniform1i(normal_toggle_var, SHOW_NORMALS);
	skybox->sendLight(terrain_shaderProgram);
	// Draw terrains
	for (int i = 0; i < isles.size(); i++) {
		isles[i]->draw(terrain_shaderProgram);
	}
	// Draw terrain outlines
	if (SHOW_OUTLINES) {
		glUseProgram(outline_shaderProgram);
		glDepthMask(GL_TRUE);
		for (int i = 0; i < isles.size(); i++) {
			isles[i]->draw(outline_shaderProgram);
		}
	}
	/********** TERRAIN END **********/



	/********** RENDER SKYBOX **********/
	glUseProgram(skybox_shaderProgram);
	skybox->sendLight(skybox_shaderProgram);
	skybox->draw(skybox_shaderProgram);
	/********** SKYBOX END **********/



	/********** RENDER WATER **********/
	glUseProgram(water_shaderProgram);
	// Adjust timer for input into water waves
	elapsedTime = (float)(clock() - tpf) / CLOCKS_PER_SEC;
	if (elapsedTime > 13.0f && go_backwards == false) {
		tpf = clock();
		time2 = elapsedTime;
		go_backwards = true;
	}
	if (go_backwards == true) {
		time2 = (time2 - 0.01f);
		elapsedTime = time2;
		if (time2 < 0.0f) {
			go_backwards = false;
			tpf = clock();
		}
	}
	// Send information to water shader
	uCam_pos = glGetUniformLocation(water_shaderProgram, "camPos");
	glUniform3f(uCam_pos, cam_pos.x, cam_pos.y, cam_pos.z);
	glUniform1f(glGetUniformLocation(water_shaderProgram, "u_time"), elapsedTime / 5.0f);
	// Draw ocean
	if (SHOW_WATER) {
		ocean->draw(water_shaderProgram);
	}
	/********** WATER END **********/



	/********** RENDER BOUNDING BOXES **********/
	if (TOGGLE_BOUNDING_BOXES) {
		glUseProgram(bounds_shaderProgram);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Set wireframe mode
		// Draw bounding box for ship
		model->bounding_box->draw(bounds_shaderProgram);
		// Draw bounding box for buoy
		//buoy->bounding_box->draw(bounds_shaderProgram);
		// Draw bounding boxes for islands
		for (int i = 0; i < isles.size(); i++) {
			isles[i]->drawBounds(bounds_shaderProgram);
		}
		for (int i = 0; i < buoys.size(); i++) {
			buoys[i]->bounding_box->draw(bounds_shaderProgram);
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Turn off wireframe after rendering bounds
	}
	/********** BOUNDING BOXES END **********/

	

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		key_down = true;
		key_pressed = key;
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		else if (key == GLFW_KEY_B) {
			if (TOGGLE_BOUNDING_BOXES == true) {
				TOGGLE_BOUNDING_BOXES = false;
			}
			else {
				TOGGLE_BOUNDING_BOXES = true;
			}
		}

		else if (key == GLFW_KEY_G) {
			sound->toggleMuteAll();
		}

		else if (key == GLFW_KEY_T) {
			std::cout << "Regenerating terrain!" << std::endl;
			for (int i = 0; i < isles.size(); i++) {
				for (int j = 0; j < isles[i]->lands.size(); j++) {
					isles[i]->lands[j]->init();
				}
			}
		}

		else if (key == GLFW_KEY_N) {
			if (SHOW_NORMALS == 0) {
				std::cout << "Normals coloring on!" << std::endl;
				SHOW_NORMALS = 1;
			}
			else {
				std::cout << "Normals coloring off!" << std::endl;
				SHOW_NORMALS = 0;
			}
		}

		else if (key == GLFW_KEY_P) {
			if (TOON_SHADING == 0) {
				std::cout << "Toon coloring on!" << std::endl;
				TOON_SHADING = 1;
			}
			else {
				std::cout << "Toon off!" << std::endl;
				TOON_SHADING = 0;
			}
		}

		else if (key == GLFW_KEY_M) {
			regen();
		}

		else if (key == GLFW_KEY_COMMA) {
			regen_single();
		}

		else if (key == GLFW_KEY_O) {
			if (SHOW_OUTLINES == false) {
				std::cout << "Toggle silhouette outlines on!" << std::endl;
				SHOW_OUTLINES = true;
			}
			else {
				std::cout << "Toggle silhouette outlines off!" << std::endl;
				SHOW_OUTLINES = false;
			}
		}

		else if (key == GLFW_KEY_0) {
			if (DEBUG == false) {
				std::cout << "Toggle DEBUG MODE on!" << std::endl;
				DEBUG = true;
			}
			else {
				std::cout << "Toggle DEBUG MODE off!" << std::endl;
				DEBUG = false;
			}
		}

		else if (key == GLFW_KEY_L) {
			if (SHOW_WATER == false) {
				std::cout << "Showing water!" << std::endl;
				SHOW_WATER = true;
			}
			else {
				std::cout << "Hiding water!" << std::endl;
				SHOW_WATER = false;
			}
		}

		else if (key == GLFW_KEY_F) {
			if (SPRAY_PARTICLES == false) {
				std::cout << "Spraying blue particles!" << std::endl;
				SPRAY_PARTICLES = true;
			}
			else {
				std::cout << "Stopped spraying particles!" << std::endl;
				SPRAY_PARTICLES = false;
			}
		}

		else if (key == GLFW_KEY_R) {
			FREE_CAMERA = false;
			GAME_OVER = false;
			PRINT_GAME_OVER = false;
			game_over_rotate = 0.0f;
			ship_angle = 0.0f;
			bool ship_collided = false;

			model->toWorld = ORIGINAL_SHIP_POS;
			model->bounding_box->toWorld = ORIGINAL_SHIP_BOUND_POS;
			model->bounding_box->update();

			cam_pos = glm::vec3(-20.3139f, 47.818f, -116.562f);
			std::cout << "\nYou have reset the ship!" << std::endl;
		}

		else if (key == GLFW_KEY_C) {
			if (!GAME_OVER) {
				if (FREE_CAMERA == true) {
					std::cout << "Free camera movement turned off!" << std::endl;
					FREE_CAMERA = false;
					cam_pos = STORED_CAM_POS;
					ship_angle = STORED_SHIP_ANGLE;
				}
				else {
					std::cout << "Free camera movement turned on!" << std::endl;
					STORED_CAM_POS = glm::vec3(cam_pos);
					STORED_SHIP_ANGLE = ship_angle;
					FREE_CAMERA = true;
				}
			}
		}
	}
	else if (action == GLFW_RELEASE) {
		key_down = false;
	}
}



/******************* CAMERA CONTROLS *******************/

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		glm::dvec2 curPos;
		glfwGetCursorPos(window, &curPos.x, &curPos.y);
		lastPoint = trackBallMapping(curPos);

		glm::mat4 rotationM;
	}
}

glm::vec3 Window::trackBallMapping(glm::dvec2 curPos) {
	glm::vec3 vec;
	float d;
	vec.x = (2.0f*curPos.x - Window::width) / Window::width;
	vec.y = (Window::height - 2.0f*curPos.y) / Window::height;
	vec.z = 0.0f;
	d = glm::length(vec);
	d = (d < 1.0) ? d : 1.0;
	vec.z = sqrtf(1.001 - d*d);
	glm::normalize(vec);
	return vec;
}

void Window::cursor_callback(GLFWwindow* window, double xpos, double ypos) {
	int leftMouse = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	int rightMouse = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

	if (leftMouse == GLFW_PRESS && FREE_CAMERA)
	{
		// Track cursor for rotation
		glm::vec3 direction;
		float roat_angle;
		glm::vec3 currentPoint;

		glm::dvec2 curPos(xpos, ypos);

		// Rotate
		currentPoint = trackBallMapping(curPos); // Get the current position on track ball
												 // Sphere location
		direction = glm::vec3(currentPoint.x - lastPoint.x, currentPoint.y - lastPoint.y, currentPoint.z - lastPoint.z);
		float velocity = glm::length(direction);

		if (velocity > 0.0001)
		{
			// Get rotation axis
			glm::vec3 rotAxis;
			rotAxis = glm::cross(lastPoint, currentPoint);
			roat_angle = velocity;

			// Get rotation matrix
			glm::mat4 rotationM = glm::rotate(glm::mat4(1.0f), roat_angle, rotAxis);
			// Rotate cam_pos
			glm::vec4 new_cam_pos = rotationM * glm::vec4(cam_pos, 1.0f);

			cam_pos = glm::vec3(new_cam_pos.x / new_cam_pos.w, new_cam_pos.y / new_cam_pos.w, new_cam_pos.z / new_cam_pos.w);
			// Rotate cam_up
			// New i vector
			
			glm::vec3 new_i = glm::normalize(cam_look_at - cam_pos);
			glm::vec3 interCam = glm::cross(cam_up, new_i);
			glm::vec3 new_cam_up = glm::normalize(glm::cross(new_i, interCam));

			V = glm::lookAt(cam_pos, cam_look_at, cam_up);
			lastPoint = currentPoint;
		}
	}
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (FREE_CAMERA) {
		cam_pos.z += yoffset;
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

// Regenerate map with islands
void Window::regen() {
	FREE_CAMERA = false;
	GAME_OVER = false;
	PRINT_GAME_OVER = false;
	game_over_rotate = 0.0f;
	ship_angle = 0.0f;
	ship_collided = false;
	NUM_COLLECTED = 0;

	model->toWorld = ORIGINAL_SHIP_POS;
	model->bounding_box->toWorld = ORIGINAL_SHIP_BOUND_POS;
	model->bounding_box->update();

	cam_pos = glm::vec3(-20.3139f, 47.818f, -116.562f);

	// Reset buoys
	for (int i = 0; i < buoys.size(); i++) {
		buoys[i]->toDraw = true;
		buoys[i]->object_color = buoy->object_color;
		buoys[i]->scale(0.2f);
		buoys[i]->toWorld = glm::mat4(1.0f);
		buoys[i]->bounding_box->toWorld = buoy->bounding_box->toWorld;
	}

	// Randomly place islands
	for (int i = 0; i < isles.size(); i++) {
		place_island = true;
		for (int k = 0; k < isles[i]->lands.size(); k++) {
			//isles[i]->lands[k]->toWorld = glm::mat4(1.0f);
			for (int l = 0; l < isles[i]->lands[k]->bounds.size(); l++) {
				isles[i]->lands[k]->bounds[l]->toWorld = glm::mat4(1.0f);
				isles[i]->lands[k]->bounds[l]->update();
			}
		}

		// Repeat if island intersects / collides
		while (place_island == true) {
			place_island = false;
			intersects_origin = false;
			float isle_x = (ISLAND_MIN_RANGE + 1) + (((float)rand()) / (float)RAND_MAX) * (ISLAND_MAX_RANGE - (ISLAND_MIN_RANGE + 1));
			float isle_z = (ISLAND_MIN_RANGE + 1) + (((float)rand()) / (float)RAND_MAX) * (ISLAND_MAX_RANGE - (ISLAND_MIN_RANGE + 1));
			std::cout << "\nIsland #" << i + 1 << std::endl << "x: " << isle_x << ", z: " << isle_z << std::endl;
			isles[i]->move(isle_x, isle_z);

			// Don't spawn island on top of ship
			place_island = isles[i]->checkBounds(tutorial_island);
			if (place_island == true) {
				std::cout << "Uh oh! Island #" << i + 1 << " is too close to the ship! Moving it someplace else..." << std::endl;
				isles[i]->move(-isle_x, -isle_z);
				intersects_origin = true;
			}

			// Don't intersect with other islands
			if (intersects_origin == false) {
				for (int j = 0; j < i; j++) {
					place_island = isles[i]->checkBounds(isles[j]);
					if (place_island == true) {
						std::cout << "Woops! Island #" << i + 1 << " is intersecting with Island #" << j + 1 << "! Trying new coordinates..." << std::endl;
						isles[i]->move(-isle_x, -isle_z);
						break;
					}
				}
			}
		}
	}
	std::cout << "\nIsland generation and placement completed!" << std::endl;

	// Randomly place buoys
	std::cout << "\nPlacing buoys..." << std::endl;
	for (int i = 0; i < buoys.size(); i++) {
		place_island = true;

		// Repeat if island intersects / collides
		while (place_island == true) {
			place_island = false;
			float isle_x = (ISLAND_MIN_RANGE + 1) + (((float)rand()) / (float)RAND_MAX) * (ISLAND_MAX_RANGE - (ISLAND_MIN_RANGE + 1));
			float isle_z = (ISLAND_MIN_RANGE + 1) + (((float)rand()) / (float)RAND_MAX) * (ISLAND_MAX_RANGE - (ISLAND_MIN_RANGE + 1));
			std::cout << "\nBuoy #" << i + 1 << std::endl << "x: " << isle_x << ", z: " << isle_z << std::endl;
			buoys[i]->translate(isle_x, 0.0f, isle_z);

			// Don't intersect with other islands
			if (intersects_origin == false) {
				for (int j = 0; j < isles.size(); j++) {
					place_island = buoys[i]->checkBounds(isles[j]);
					if (place_island == true) {
						std::cout << "Woops! Buoy #" << i + 1 << " is intersecting with Island #" << j + 1 << "! Trying new coordinates..." << std::endl;
						buoys[i]->translate(-isle_x, 0.0f, -isle_z);
						break;
					}
				}
			}
		}
	}


	for (int i = 0; i < buoys.size(); i++) {
		buoys[i]->scale(0.2f);
	}

}

// Regenerate map, but with only singular islands
void Window::regen_single() {
	FREE_CAMERA = false;
	GAME_OVER = false;
	PRINT_GAME_OVER = false;
	game_over_rotate = 0.0f;
	ship_angle = 0.0f;
	ship_collided = false;
	NUM_COLLECTED = 0;

	model->toWorld = ORIGINAL_SHIP_POS;
	model->bounding_box->toWorld = ORIGINAL_SHIP_BOUND_POS;
	model->bounding_box->update();

	cam_pos = glm::vec3(-20.3139f, 47.818f, -116.562f);

	// Reset buoys
	for (int i = 0; i < buoys.size(); i++) {
		buoys[i]->toDraw = true;
		buoys[i]->object_color = buoy->object_color;
		buoys[i]->scale(0.2f);
		buoys[i]->toWorld = glm::mat4(1.0f);
		buoys[i]->bounding_box->toWorld = buoy->bounding_box->toWorld;
	}

	// Randomly place islands
	for (int i = 0; i < isles.size(); i++) {
		place_island = true;
		for (int k = 0; k < isles[i]->lands.size(); k++) {
			isles[i]->lands[k]->toWorld = glm::mat4(1.0f);
			for (int l = 0; l < isles[i]->lands[k]->bounds.size(); l++) {
				isles[i]->lands[k]->bounds[l]->toWorld = glm::mat4(1.0f);
				isles[i]->lands[k]->bounds[l]->update();
			}
		}

		// Repeat if island intersects / collides
		while (place_island == true) {
			place_island = false;
			intersects_origin = false;
			float isle_x = (ISLAND_MIN_RANGE + 1) + (((float)rand()) / (float)RAND_MAX) * (ISLAND_MAX_RANGE - (ISLAND_MIN_RANGE + 1));
			float isle_z = (ISLAND_MIN_RANGE + 1) + (((float)rand()) / (float)RAND_MAX) * (ISLAND_MAX_RANGE - (ISLAND_MIN_RANGE + 1));
			std::cout << "\nIsland #" << i + 1 << std::endl << "x: " << isle_x << ", z: " << isle_z << std::endl;
			isles[i]->move(isle_x, isle_z);

			// Don't spawn island on top of ship
			place_island = isles[i]->checkBounds(tutorial_island);
			if (place_island == true) {
				std::cout << "Uh oh! Island #" << i + 1 << " is too close to the ship! Moving it someplace else..." << std::endl;
				isles[i]->move(-isle_x, -isle_z);
				intersects_origin = true;
			}

			// Don't intersect with other islands
			if (intersects_origin == false) {
				for (int j = 0; j < i; j++) {
					place_island = isles[i]->checkBounds(isles[j]);
					if (place_island == true) {
						std::cout << "Woops! Island #" << i + 1 << " is intersecting with Island #" << j + 1 << "! Trying new coordinates..." << std::endl;
						isles[i]->move(-isle_x, -isle_z);
						break;
					}
				}
			}
		}
	}
	std::cout << "\nIsland generation and placement completed!" << std::endl;

	// Randomly place buoys
	std::cout << "\nPlacing buoys..." << std::endl;
	for (int i = 0; i < buoys.size(); i++) {
		place_island = true;

		// Repeat if island intersects / collides
		while (place_island == true) {
			place_island = false;
			float isle_x = (ISLAND_MIN_RANGE + 1) + (((float)rand()) / (float)RAND_MAX) * (ISLAND_MAX_RANGE - (ISLAND_MIN_RANGE + 1));
			float isle_z = (ISLAND_MIN_RANGE + 1) + (((float)rand()) / (float)RAND_MAX) * (ISLAND_MAX_RANGE - (ISLAND_MIN_RANGE + 1));
			std::cout << "\nBuoy #" << i + 1 << std::endl << "x: " << isle_x << ", z: " << isle_z << std::endl;
			buoys[i]->translate(isle_x, 0.0f, isle_z);

			// Don't intersect with other islands
			if (intersects_origin == false) {
				for (int j = 0; j < isles.size(); j++) {
					place_island = buoys[i]->checkBounds(isles[j]);
					if (place_island == true) {
						std::cout << "Woops! Buoy #" << i + 1 << " is intersecting with Island #" << j + 1 << "! Trying new coordinates..." << std::endl;
						buoys[i]->translate(-isle_x, 0.0f, -isle_z);
						break;
					}
				}
			}
		}
	}


	for (int i = 0; i < buoys.size(); i++) {
		buoys[i]->scale(0.2f);
	}

}