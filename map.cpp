#include "map.h"

Map::Map(char* file, long x, long y, char* mapN, char* mapNW, char* mapW, char* mapSW, char* mapS, char* mapSE, char* mapE, char* mapNE) {
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