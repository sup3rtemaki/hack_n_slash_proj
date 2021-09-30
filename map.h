#ifndef MAP
#define MAP

#include <string>

class Map {
public:
	std::string file;
	long mapPosX, mapPosY;
	std::string mapN, mapNW, mapW, mapSW, mapS, mapSE, mapE,mapNE;

	Map();
	Map(std::string file, int x, int y, std::string mapN, std::string mapNW, std::string mapW, std::string mapSW, std::string mapS, std::string mapSE, std::string mapE, std::string mapNE);
};

#endif // !MAP
