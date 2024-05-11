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

	vector<tuple<int, int, int>> enemies; // <id, x, y>
	vector<pair<bool, tuple<int, int, int>>> itemsInMap; // <isPickedUp, <id, x, y>>

	struct Waypoint {
		SDL_Rect waypointRect;
		int xDestination;
		int yDestination;
		string nextMapFile;
	};

	Waypoint nextMapWaypoint;

	vector<Waypoint> currentMapWaypoints;
	
	//EXEMPLO
	//std::tie(enemyId, enemyX, enemyY) = enemies[0];

	Map();
};

#endif // !MAP
