/****************************/
/*****	by ANDY THAI	*****/
/****************************/

#pragma once
#include <vector>
#include "Bound.h"
#include "Terrain.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

class Island
{
public:
	Island();
	Island(int size, int color);
	~Island();
	void draw(GLuint shaderProgram);
	void drawBounds(GLuint shaderProgram);
	void move(float x, float z);
	void update();
	bool checkBounds(Island * other);

	bool collide = false;
	int isle_size = 1;
	int isle_color = 1;
	std::vector<Terrain*> lands;


};

