#include "map.h"
#include "cJSON.h"

#include <iostream>
#include <fstream>
#include <string>

//=================================================================

Map::Map(int id, std::string file, int leftX1, int leftY1, int leftX2, int leftY2,
	int rightX1, int rightY1, int rightX2, int rightY2,
	int topX1, int topY1, int topX2, int topY2,
	int bottomX1, int bottomY1, int bottomX2, int bottomY2,
	int leftMapId, int rightMapId, int topMapId, int bottomMapId, int qtEnemies,
	std::vector<std::tuple<int, int, int>> enemies) {

	this->id = id;
	this->file = file;

	this->leftX1 = leftX1;
	this->leftY1 = leftY1;
	this->leftX2 = leftX2;
	this->leftY2 = leftY2;

	this->rightX1 = rightX1;
	this->rightY1 = rightY1;
	this->rightX2 = rightX2;
	this->rightY2 = rightY2;

	this->topX1 = topX1;
	this->topY1 = topY1;
	this->topX2 = topX2;
	this->topY2 = topY2;

	this->bottomX1 = bottomX1;
	this->bottomY1 = bottomY1;
	this->bottomX2 = bottomX2;
	this->bottomY2 = bottomY2;

	this->leftMapId = leftMapId;
	this->rightMapId = rightMapId;
	this->topMapId = topMapId;
	this->bottomMapId = bottomMapId;

	this->qtEnemies = qtEnemies;
	this->enemies = enemies;
}


Map::Map() {
	this->id = 0;
	this->file = "";

	this->leftX1 = -1;
	this->leftY1 = -1;
	this->leftX2 = -1;
	this->leftY2 = -1;

	this->rightX1 = -1;
	this->rightY1 = -1;
	this->rightX2 = -1;
	this->rightY2 = -1;

	this->topX1 = -1;
	this->topY1 = -1;
	this->topX2 = -1;
	this->topY2 = -1;

	this->bottomX1 = -1;
	this->bottomY1 = -1;
	this->bottomX2 = -1;
	this->bottomY2 = -1;

	this->leftMapId = -1;
	this->rightMapId = -1;
	this->topMapId = -1;
	this->bottomMapId = -1;
};