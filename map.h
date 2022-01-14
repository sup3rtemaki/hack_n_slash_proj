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
	int leftX1, leftY1, leftX2, leftY2;
	int rightX1, rightY1, rightX2, rightY2;
	int topX1, topY1, topX2, topY2;
	int bottomX1, bottomY1, bottomX2, bottomY2;
	int leftMapId, rightMapId, topMapId, bottomMapId;
	int qtEnemies;
	//TODO Criar lista de inimigos

	Map();
	Map(int id, std::string file, int leftX1, int leftY1, int leftX2, int leftY2,
		int rightX1, int rightY1, int rightX2, int rightY2,
		int topX1, int topY1, int topX2, int topY2,
		int bottomX1, int bottomY1, int bottomX2, int bottomY2,
		int leftMapId, int rightMapId, int topMapId, int bottomMapId);
};

#endif // !MAP
