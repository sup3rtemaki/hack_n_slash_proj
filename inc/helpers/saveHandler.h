#ifndef SAVEHANDLER
#define SAVEHANDLER

#include "itemsHub.h"

#include <vector>

class SaveHandler{
public:
	SaveHandler();
	~SaveHandler();

	void save(int heroHp, int heroX, int heroY, int currentMapId, std::vector<std::pair<int, int>> items);
	bool load();

	int getHeroHp();
	int getHeroX();
	int getHeroY();
	int getCurrentMapId();
	std::vector<std::pair<int, int>> getItems();

private:
	int heroHp;
	int heroX;
	int heroY;
	int currentMapId;
	std::vector<std::pair<int, int>> items;
};

#endif // !SAVEHANDLER
