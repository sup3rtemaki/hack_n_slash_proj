#ifndef MAP
#define MAP

class Map {
public:
	char *file;
	long mapPosX, mapPosY;
	char *mapN, *mapNW, *mapW, *mapSW, *mapS, *mapSE, *mapE,*mapNE;
	Map(char* file, long x, long y, char* mapN, char* mapNW, char* mapW, char* mapSW, char* mapS, char* mapSE, char* mapE, char* mapNE);
};

#endif // !MAP
