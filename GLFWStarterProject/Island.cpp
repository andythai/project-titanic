/****************************/
/*****	by ANDY THAI	*****/
/****************************/

#include "Island.h"


Island::Island()
{
	lands.push_back(new Terrain());
	lands[0]->init();
}

Island::Island(int size, int color) {
	isle_size = size;
	isle_color = color;

	// Generate terrains
	if (isle_size > 1) {
		for (int i = 0; i < isle_size; i++) {
			for (int j = 0; j < isle_size; j++) {
				// Creates terrain tiles
				int terrain_type = rand() % 3 + 1;
				Terrain * terrain = new Terrain(terrain_type, isle_color);
				terrain->init();
				terrain->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(79 * j, 0, 79 * i)) * terrain->toWorld;
				
				// Updates bounding boxes
				for (int k = 0; k < terrain->bounds.size(); k++) {
					terrain->bounds[k]->update();
				}
				lands.push_back(terrain);
			}
		}
	}
	
	else {
		lands.push_back(new Terrain());
		lands[0]->init();
	}
}

Island::~Island()
{
	for (int i = 0; i < lands.size(); i++) {
		delete(lands[i]);
	}
}

void Island::draw(GLuint shaderProgram) {
	for (int i = 0; i < lands.size(); i++) {
		lands[i]->draw(shaderProgram);
	}
}

void Island::move(float x, float z) {
	for (int i = 0; i < lands.size(); i++) {
		lands[i]->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0, z)) * lands[i]->toWorld;
		/*
		for (int j = 0; j < lands[i]->bounds.size(); j++) {
			lands[i]->bounds[j]->update();
		}*/
	}
	update();
}

void Island::drawBounds(GLuint shaderProgram) {
	for (int i = 0; i < lands.size(); i++) {
		for (int j = 0; j < lands[i]->bounds.size(); j++) {
			lands[i]->bounds[j]->draw(shaderProgram);
		}
	}
}

void Island::update() {
	for (int i = 0; i < lands.size(); i++) {
		for (int j = 0; j < lands[i]->bounds.size(); j++) {
			lands[i]->bounds[j]->toWorld = lands[i]->toWorld;
			if (j == 1) {
				lands[i]->bounds[1]->toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 14.0f, 0.0f)) * lands[i]->toWorld;
			}
			lands[i]->bounds[j]->update();
		}
	}
}

bool Island::checkBounds(Island * other) {
	for (int i = 0; i < lands.size(); i++) {
		for (int j = 0; j < lands[i]->bounds.size(); j++) {
			for (int k = 0; k < other->lands.size(); k++) {
				for (int l = 0; l < other->lands[k]->bounds.size(); l++) {
					bool check = lands[i]->bounds[j]->check_collision(other->lands[k]->bounds[l]);
					if (check == true) {
						return true;
					}
				}
			}
		}
	}
	return false;
}