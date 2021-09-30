#include "map.h"

Map::Map(std::string file, int x, int y, std::string mapN, std::string mapNW, std::string mapW, std::string mapSW, std::string mapS, std::string mapSE, std::string mapE, std::string mapNE) {
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
	this->file = "";
	this->mapPosX = 0;
	this->mapPosY = 0;
	this->mapN = "";
	this->mapNW = "";
	this->mapW = "";
	this->mapSW = "";
	this->mapS = "";
	this->mapSE = "";
	this->mapE = "";
	this->mapNE = "";
};