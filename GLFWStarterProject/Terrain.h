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

static const int MAPSIZE = 256 + 1; // must be 2^n + 1
static const float SIZE_X = 80.0;
static const float SIZE_Z = 80.0;
static const float DS_RANGE = 10;

class Terrain {
public:
    glm::mat4 toWorld = glm::mat4(1.0f);
    float heightMap[MAPSIZE][MAPSIZE];
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    // std::vector<unsigned int> triangle;
    // bool isUpperLeftTriangle = true;
    
    unsigned char* texture;
    GLuint HMVAO, HMVBO, IDVBO, NBO;
    GLuint uProjection, uModelview;
    
    Terrain();
    ~Terrain();
    void init();
    void diamondSquare(int minX, int minY, int maxX, int maxY, float range);
    void generateVertices();
    void generateIndices();
    void calcNormals();
    void draw(GLuint shaderProgram);
    
    unsigned char* loadPPM(const char* filename, int& width, int& height);
};

#endif /* Terrain_hpp */
