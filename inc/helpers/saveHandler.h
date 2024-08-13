#ifndef SAVEHANDLER
#define SAVEHANDLER

#include "itemsHub.h"

#include <vector>

class SaveHandler{
public:
	SaveHandler();
	~SaveHandler();

	void save(
		int heroHp,
		int heroX,
		int heroY,
		string currentMapFile,
		std::vector<std::pair<int, int>> items,
		std::vector<int>openDoorsIds);
	bool load();

	int getHeroHp();
	int getHeroX();
	int getHeroY();
	string getCurrentMapFile();
	void setCurrentMapFile(string mapFile);
	std::vector<std::pair<int, int>> getItems();
	std::vector<int> getOpenDoorsIds();

private:
	int heroHp;
	int heroX;
	int heroY;
	string currentMapFile;
	std::vector<std::pair<int, int>> items;
	std::vector<int> openDoorsIds;
};

#endif // !SAVEHANDLER
