//
//  Terrain.hpp
//  CSE167Final
//
//  Created by Tommy Yang on 12/1/16.
//  Copyright © 2016 tOMG. All rights reserved.
//

#ifndef Terrain_hpp
#define Terrain_hpp

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>
#include <vector>
#include <string>

class Terrain {
public:
    glm::mat4 toWorld = glm::mat4(1.0f);
    int mapSize = 256 + 1; // must be 2^n + 1
    float dispSizeX = 80.0;
    float dispSizeZ = 80.0;
    int terrainType = 1;
	int terrainColor = 1;
    int lBound = -10;
    int uBound = 10;

	// Bounding box
	Bound * box1 = new Bound(20.0f, 2.5f, 20.0f);
	Bound * mountain_box = new Bound(15.0f, 9.5f, 15.0f); //5.5f

	std::vector<Bound*> bounds;

    std::vector<std::vector<int>> heightMap;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;

    unsigned char* texture;
    GLuint HMVAO, HMVBO, IDVBO, NBO;
    GLuint uProjection, uModelview, uTerrainType, uTerrainColor, uModel, uView;

    // GLuint textureID;
    // GLuint uNormalToggle;

    Terrain();
    Terrain(int type, int color);
    ~Terrain();
    void init();
    void diamondSquare(int minX, int minY, int maxX, int maxY, float lowerBound, float upperBound);
    void adjust();
    void generateVertices();
    void generateIndices();
    void calcNormals();
    void draw(GLuint shaderProgram);

    unsigned char* loadPPM(const char* filename, int& width, int& height);
};

#endif /* Terrain_hpp */
