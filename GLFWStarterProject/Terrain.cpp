//
//  Terrain.cpp
//  CSE167Final
//
//  Diamond-Square: http://gamedev.stackexchange.com/questions/37389/diamond-square-terrain-generation-problem
//  Generate Indices: http://www.learnopengles.com/tag/triangle-strips/ http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=8


#include <iostream> // std::cerr
#include <random>
#include "Window.h"
#include "Terrain.h"

std::random_device rd;
std::default_random_engine generator(rd());

Terrain::Terrain() {
    // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
    glGenVertexArrays(1, &HMVAO);
    glGenBuffers(1, &HMVBO);
    glGenBuffers(1, &IDVBO);
    // glGenBuffers(1, &NBO);
    
    /*
    texture = loadPPM(filename, width, height);
    if (texture == nullptr) {
     std::cerr << "loadPPM error" << std::endl;
    }
    */
    
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0);
    
    init();
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
    heightMap[0][MAPSIZE-1] = 0;
    heightMap[MAPSIZE-1][0] = 0;
    heightMap[MAPSIZE-1][MAPSIZE-1] = 0;
    
    diamondSquare(0, 0, MAPSIZE - 1, MAPSIZE - 1, DS_RANGE);
    
    generateVertices();
    generateIndices();
    
    glBindVertexArray(HMVAO);
    glBindBuffer(GL_ARRAY_BUFFER, HMVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IDVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(MAPSIZE * MAPSIZE);
    
    /*
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*) 0);
    */
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind the VAO now so we don't accidentally tamper with it.
    glBindVertexArray(0);
}

void Terrain::generateVertices() {
    vertices.resize(MAPSIZE * MAPSIZE);
    for (int x = 0; x < MAPSIZE; x++) {
        for (int y = 0; y < MAPSIZE; y++) {
            int i = (x * MAPSIZE) + y;
            float xVal = i % MAPSIZE;
            float zVal = (float) i / MAPSIZE;
            vertices[i] = glm::vec3(-SIZE_X / 2 + SIZE_X * xVal / (MAPSIZE - 1), heightMap[x][y], -SIZE_Z / 2 + SIZE_Z * zVal / (MAPSIZE - 1));
        }
    }
}

void Terrain::generateIndices() {
    // Now build the index data
    int numStrips = MAPSIZE - 1;
    int numDegens = numStrips;
    int verticesPerStrip = 2 * MAPSIZE;
    
    indices.resize(verticesPerStrip * numStrips + numDegens);
    
    int offset = 0;
    for (int y = 0; y < MAPSIZE - 1; y++) {
        for (int x = 0; x < MAPSIZE; x++) {
            // One part of the strip
            indices[offset++] = (y * MAPSIZE) + x;
            // triangle.push_back((y * MAPSIZE) + x);
            indices[offset++] = ((y + 1) * MAPSIZE) + x;
            // triangle.push_back(((y + 1) * MAPSIZE) + x);
        }
        
        calcNormals(); // for this strip
        // Degenerate end: repeat last vertex
        indices[offset++] = MAPSIZE * MAPSIZE;
    }
}

void Terrain::diamondSquare(int minX, int minY, int maxX, int maxY, float range) {
    std::uniform_real_distribution<float> distribution(-range, range);
    
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
    
    range = range / 2;
    
    if (midX - minX > 0) {
        diamondSquare(minX, minY, midX, midY, range);
        diamondSquare(minX, midY, midX, maxY, range);
        diamondSquare(midX, minY, maxX, midY, range);
        diamondSquare(midX, midY, maxX, maxY, range);
    }
}

void Terrain::calcNormals() {
    /*
    while (triangle.size() > 3) {
        glm::vec3 p0, p1, p2;
        if (isUpperLeftTriangle) {
            p0 = vertices[triangle[0]];
            p1 = vertices[triangle[1]];
            p2 = vertices[triangle[2]];
        }
        else {
            p0 = vertices[triangle[1]];
            p1 = vertices[triangle[0]];
            p2 = vertices[triangle[2]];
        }
        glm::vec3 vector0 = p2 - p1;
        glm::vec3 vector1 = p0 - p1;
        
        glm::vec3 normal = glm::normalize(vector0 * vector1);
        normals.push_back(normal);
        
        isUpperLeftTriangle = !isUpperLeftTriangle;
        triangle.erase(triangle.begin());
    }
    triangle.erase(triangle.begin(), triangle.end());
    */
    normals.resize(MAPSIZE * MAPSIZE);
    for (int x = 0; x < MAPSIZE - 1;x++) {
        for (int y = 0; y < MAPSIZE - 1; y++) {

            glm::vec3 n;
            glm::vec3 n2;
            glm::vec3 v1 = vertices[(x * MAPSIZE) + y];
            glm::vec3 v2 = vertices[((x + 1) * MAPSIZE) + y];
            glm::vec3 v3 = vertices[(x * MAPSIZE) + y + 1];
            glm::vec3 v4 = vertices[((x + 1) * MAPSIZE) + y + 1];
            if(x % 2 == 0) {
                glm::vec3 n = glm::normalize((v3 - v2) * (v1 - v2));
                normals[(x * MAPSIZE) + y] = n;
            }
            else {
                glm::vec3 n = glm::normalize((v2 - v3) * (v4 - v3));
                normals[(x * MAPSIZE) + y] = n;
            }
        }
    }
    
    std::cerr << normals.size() << std::endl;
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
    glBindVertexArray(HMVAO);
    glDrawElements(GL_TRIANGLE_STRIP, MAPSIZE * (MAPSIZE - 1) * 2 + MAPSIZE - 1, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

/** Load a ppm file from disk.
 @input filename The location of the PPM file.  If the file is not found, an error message
 will be printed and this function will return 0
 @input width This will be modified to contain the width of the loaded image, or 0 if file not found
 @input height This will be modified to contain the height of the loaded image, or 0 if file not found
 @return Returns the RGB pixel data as interleaved unsigned chars (R0 G0 B0 R1 G1 B1 R2 G2 B2 .... etc) or 0 if an error ocured
 **/
/*
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
 */
