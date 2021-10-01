#include "map.h"

Map::Map(int id, std::string file, int x, int y, int mapN, int mapNW, int mapW, int mapSW, int mapS, int mapSE, int mapE, int mapNE) {
	this->id = id;
	this->file = file;
	this->mapPosX = x;
	this->mapPosY = y;
	this->mapN = mapN;
	this->mapNW = mapNW;
	this->mapW = mapW;
	this->mapSW = mapSW;
	this->mapS = mapS;
	this->mapSE = mapSE;
	this->mapE = mapE;
	this->mapNE = mapNE;
}

Map::Map() {
	this->id = 0;
	this->file = "";
	this->mapPosX = 0;
	this->mapPosY = 0;
	this->mapN = 99;
	this->mapNW = 99;
	this->mapW = 99;
	this->mapSW = 99;
	this->mapS = 99;
	this->mapSE = 99;
	this->mapE = 99;
	this->mapNE = 99;
};