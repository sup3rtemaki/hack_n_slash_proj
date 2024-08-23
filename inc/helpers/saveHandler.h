#ifndef SAVEHANDLER
#define SAVEHANDLER

#include "itemsHub.h"

#include <vector>

struct BloodstainInfo {
	int x;
	int y;
	int essence;
	string mapName;
};

class SaveHandler{
public:
	SaveHandler();
	~SaveHandler();

	void save(
		int heroHp,
		int heroX,
		int heroY,
		int essence,
		string currentMapFile,
		std::vector<std::pair<int, int>> items,
		std::vector<int>openDoorsIds,
		std::vector<int> defeatedBossesIds,
		int bloodstainX,
		int bloodstainY,
		int bloodstainEssence,
		string bloodstainMapName);
	bool load();

	int getHeroHp();
	int getHeroX();
	int getHeroY();
	int getEssence();
	string getCurrentMapFile();
	void setCurrentMapFile(string mapFile);
	std::vector<std::pair<int, int>> getItems();
	std::vector<int> getOpenDoorsIds();
	std::vector<int> getDefeatedBossesIds();
	BloodstainInfo getBloodstainInfo();

private:
	int heroHp;
	int heroX;
	int heroY;
	int essence;
	string currentMapFile;
	std::vector<std::pair<int, int>> items;
	std::vector<int> openDoorsIds;
	std::vector<int> defeatedBossesIds;
	BloodstainInfo bloodtsainInfo;
};

#endif // !SAVEHANDLER
