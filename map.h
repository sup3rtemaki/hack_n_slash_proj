#ifndef MAP
#define MAP

/*
* 00000
* 01110
* 01x10
* 01110
* 00000
*
* Based on distance to the player: If map is location is < distance render map
* else discard/unrender map
*
* 0 - map NOT to render
* 1 - map to render
* x - map where player is
*/

#include <string>

class Map {
public:
	int id;
	std::string file;
	int mapPosX, mapPosY;
	int mapN, mapNW, mapW, mapSW, mapS, mapSE, mapE,mapNE;

	Map();
	Map(int id, std::string file, int x, int y, int mapN, int mapNW, int mapW, int mapSW, int mapS, int mapSE, int mapE, int mapNE);
};

#endif // !MAP
