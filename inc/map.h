#ifndef MAP
#define MAP

#include <string>
#include <tuple>
#include <vector>

#include "entity.h"

class Map {
public:
	int id;
	string file;
	int leftX1, leftY1, leftX2, leftY2;
	int rightX1, rightY1, rightX2, rightY2;
	int topX1, topY1, topX2, topY2;
	int bottomX1, bottomY1, bottomX2, bottomY2;
	int leftMapId, rightMapId, topMapId, bottomMapId;
	int qtEnemies;

	vector<tuple<int, int, int>> enemies; // <id, x, y>
	vector<pair<bool, tuple<int, int, int>>> itemsInMap; // <isPickedUp, <id, x, y>>
	
	//EXEMPLO
	//std::tie(enemyId, enemyX, enemyY) = enemies[0];

	Map();
	Map(int id, string file, int leftX1, int leftY1, int leftX2, int leftY2,
		int rightX1, int rightY1, int rightX2, int rightY2,
		int topX1, int topY1, int topX2, int topY2,
		int bottomX1, int bottomY1, int bottomX2, int bottomY2,
		int leftMapId, int rightMapId, int topMapId, int bottomMapId, int qtEnemies,
		vector<tuple<int, int, int>> enemies);
};

#endif // !MAP
