//
//  Terrain.cpp
//  CSE167Final
//
//  Diamond-Square: http://gamedev.stackexchange.com/questions/37389/diamond-square-terrain-generation-problem
//  Generate Indices: http://www.learnopengles.com/tag/triangle-strips/ http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=8

#define _CRT_SECURE_NO_DEPRECATE
#include <iostream> // std::cerr
#include <random>
#include <cmath> // abs
#include "../Window.h"
#include "Terrain.h"

std::random_device rd;
std::default_random_engine generator(rd());

Terrain::Terrain() {
    switch (terrainType) {
    case 1: // snow mountain
        mapSize = 128 + 1;
        lBound = 0;
        uBound = 0.1;
        dispSizeX = 40;
        dispSizeZ = 40;
        break;
    case 2: // dessert
        mapSize = 128 + 1;
        lBound = 0;
        uBound = 7;
        dispSizeX = 80;
        dispSizeZ = 80;
        break;
    case 3:
        mapSize = 256 + 1;
        lBound = 0;
        uBound = 5;
        dispSizeX = 80;
        dispSizeZ = 80;
    default:
        break;
    }
    heightMap.resize(mapSize);
    for (int i = 0; i < mapSize; i++) {
        heightMap[i].resize(mapSize);
    }

    // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
    glGenVertexArrays(1, &HMVAO);
    glGenBuffers(1, &HMVBO);
    glGenBuffers(1, &IDVBO);
    glGenBuffers(1, &NBO);

    /*
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    int width, height;
    unsigned char* image = loadPPM("skybox/front.ppm", width, height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glBindTexture(GL_TEXTURE_2D, 0);
    */

    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0);

    init();
}

Terrain::Terrain(int type) {
    terrainType = type;
    Terrain();
}

Terrain::~Terrain() {
    // Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a
    // large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
    glDeleteVertexArrays(1, &HMVAO);
    glDeleteBuffers(1, &HMVBO);
    glDeleteBuffers(1, &IDVBO);
    glDeleteBuffers(1, &NBO);
}

void Terrain::init() {
    // init hm corner values
    heightMap[0][0] = 0;
    heightMap[0][mapSize-1] = 0;
    heightMap[mapSize-1][0] = 0;
    heightMap[mapSize-1][mapSize-1] = 0;

    diamondSquare(0, 0, mapSize - 1, mapSize - 1, lBound, uBound);
    adjust();

    generateVertices();
    generateIndices();

    glBindVertexArray(HMVAO);

    glBindBuffer(GL_ARRAY_BUFFER, HMVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IDVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(mapSize * mapSize);

    /** NORMALS **/
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind the VAO now so we don't accidentally tamper with it.
    glBindVertexArray(0);
}

void Terrain::generateVertices() {
    vertices.resize(mapSize * mapSize);
    for (int x = 0; x < mapSize; x++) {
        for (int y = 0; y < mapSize; y++) {
            int i = (x * mapSize) + y;
            float xVal = i % mapSize;
            float zVal = (float) i / mapSize;
            vertices[i] = glm::vec3(-dispSizeX / 2 + dispSizeX * xVal / (mapSize - 1), heightMap[x][y], -dispSizeZ / 2 + dispSizeZ * zVal / (mapSize - 1));
        }
    }
}

void Terrain::generateIndices() {
    // Now build the index data
    int numStrips = mapSize - 1;
    int numDegens = numStrips;
    int verticesPerStrip = 2 * mapSize;

    indices.resize(verticesPerStrip * numStrips + numDegens);

    int offset = 0;
    for (int y = 0; y < mapSize - 1; y++) {
        for (int x = 0; x < mapSize; x++) {
            // One part of the strip
            indices[offset++] = (y * mapSize) + x;
            // triangle.push_back((y * MAPSIZE) + x);
            indices[offset++] = ((y + 1) * mapSize) + x;
            // triangle.push_back(((y + 1) * MAPSIZE) + x);
        }

        calcNormals(); // for this strip
        // Degenerate end: repeat last vertex
        indices[offset++] = mapSize * mapSize;
    }
}

void Terrain::diamondSquare(int minX, int minY, int maxX, int maxY, float lowerBound, float upperBound) {
    std::uniform_real_distribution<float> distribution(lowerBound, upperBound);

    int midX = (minX + maxX) / 2;
    int midY = (minY + maxY) / 2;

    float minXminY = heightMap[minX][minY];
    float maxXminY = heightMap[maxX][minY];
    float minXmaxY = heightMap[minX][maxY];
    float maxXmaxY = heightMap[maxX][maxY];

    float dmPt = (minXminY + maxXminY + minXmaxY + maxXmaxY) / 4 + distribution(generator);
    heightMap[midX][midY] = dmPt;

    heightMap[minX][midY] = (minXminY + minXmaxY + dmPt) / 3 + distribution(generator); // up
    heightMap[maxX][midY] = (maxXminY + maxXmaxY + dmPt) / 3 + distribution(generator); // down
    heightMap[midX][minY] = (minXminY + maxXminY + dmPt) / 3 + distribution(generator); // left
    heightMap[midX][maxY] = (minXmaxY + maxXmaxY + dmPt) / 3 + distribution(generator); // right

    lowerBound = lowerBound / 2;
    upperBound = upperBound / 2;

    if (midX - minX > 0) {
        diamondSquare(minX, minY, midX, midY, lowerBound, upperBound);
        diamondSquare(minX, midY, midX, maxY, lowerBound, upperBound);
        diamondSquare(midX, minY, maxX, midY, lowerBound, upperBound);
        diamondSquare(midX, midY, maxX, maxY, lowerBound, upperBound);
    }
}

void Terrain::adjust() {
    if (terrainType == 1) {
        std::uniform_real_distribution<float> distribution(-1, 1);
        for (int i = 0; i < mapSize; i++) {
            for (int j = 0; j < mapSize; j++) {
                int mid = mapSize / 2;
                heightMap[i][j] += (mid - abs(mid - i)) * (mid - abs(mid - j)) / (mapSize * mapSize / 128) + distribution(generator);
            }
        }
    }
    else if (terrainType == 2) {
        std::uniform_real_distribution<float> distribution(-9.5, -9);
        for (int i = mapSize / 4; i < mapSize * 3 / 4; i++) {
            for (int j = mapSize / 4; j < mapSize * 3 / 4; j++) {
                heightMap[i][j] += distribution(generator);
            }
        }
    }
    else if (terrainType == 3) {
        std::uniform_real_distribution<float> distribution(3, 5);
        for (int i = 0; i < mapSize; i += 2) {
            for (int j = 0; j < mapSize; j += 2) {
                heightMap[i][j] += distribution(generator);
            }
        }
    }

    for (int i = 0; i < mapSize; i++) {
        heightMap[0][i] = 0;
        heightMap[mapSize - 1][i] = 0;
        heightMap[i][0] = 0;
        heightMap[i][mapSize - 1] = 0;
    }
}

void Terrain::calcNormals() {
    normals.resize(mapSize * mapSize);
    for (int x = 0; x < mapSize - 1;x++) {
        for (int y = 0; y < mapSize - 1; y++) {

            glm::vec3 n;
            glm::vec3 n2;
            glm::vec3 v1 = vertices[(x * mapSize) + y];
            glm::vec3 v2 = vertices[((x + 1) * mapSize) + y];
            glm::vec3 v3 = vertices[(x * mapSize) + y + 1];
            glm::vec3 v4 = vertices[((x + 1) * mapSize) + y + 1];
            if(x % 2 == 0) {
                glm::vec3 n = glm::normalize((v3 - v2) * (v1 - v2));
                normals[(x * mapSize) + y] = n;
            }
            else {
                glm::vec3 n = glm::normalize((v2 - v3) * (v4 - v3));
                normals[(x * mapSize) + y] = n;
            }
        }
    }
}

void Terrain::draw(GLuint shaderProgram) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Calculate the combination of the model and view (camera inverse) matrices
    glm::mat4 modelview = Window::V * toWorld;
    // We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
    // Consequently, we need to forward the projection, view, and model matrices to the shader programs
    // Get the location of the uniform variables "projection" and "modelview"
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    uModelview = glGetUniformLocation(shaderProgram, "modelview");
    // Now send these values to the shader program
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
    glUniform1i(glGetUniformLocation(shaderProgram, "type"), terrainType);
    glBindVertexArray(HMVAO);
    // glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawElements(GL_TRIANGLE_STRIP, mapSize * (mapSize - 1) * 2 + mapSize - 1, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

/** Load a ppm file from disk.
 @input filename The location of the PPM file.  If the file is not found, an error message
 will be printed and this function will return 0
 @input width This will be modified to contain the width of the loaded image, or 0 if file not found
 @input height This will be modified to contain the height of the loaded image, or 0 if file not found
 @return Returns the RGB pixel data as interleaved unsigned chars (R0 G0 B0 R1 G1 B1 R2 G2 B2 .... etc) or 0 if an error ocured
 **/
unsigned char* Terrain::loadPPM(const char* filename, int& width, int& height)
{
    const int BUFSIZE = 128;
    FILE* fp;
    unsigned int read;
    unsigned char* rawData;
    char buf[3][BUFSIZE];
    char* retval_fgets;
    size_t retval_sscanf;

    if ((fp = fopen(filename, "rb")) == NULL) {
        std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
        width = 0;
        height = 0;
        return NULL;
    }

    // Read magic number:
    retval_fgets = fgets(buf[0], BUFSIZE, fp);

    // Read width and height:
    do {
        retval_fgets = fgets(buf[0], BUFSIZE, fp);
    } while (buf[0][0] == '#');
    retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
    width = atoi(buf[1]);
    height = atoi(buf[2]);

    // Read maxval:
    do {
        retval_fgets = fgets(buf[0], BUFSIZE, fp);
    } while (buf[0][0] == '#');

    // Read image data:
    rawData = new unsigned char[width * height * 3];
    read = fread(rawData, width * height * 3, 1, fp);
    fclose(fp);
    if (read != 1) {
        std::cerr << "error parsing ppm file, incomplete data" << std::endl;
        delete[] rawData;
        width = 0;
        height = 0;
        return NULL;
    }

    return rawData;
}
