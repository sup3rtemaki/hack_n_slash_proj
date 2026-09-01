#ifndef GAMESAVEMANAGER
#define GAMESAVEMANAGER

#include <map>
#include <memory>
#include <vector>
#include <string>
#include "saveHandler.h"

class Hero;
class Item;
class Map;
class Bloodstain;

// Orchestrates save/load of game state
// Marshals data between Game, Hero, SaveHandler, and Item factory
class GameSaveManager {
public:
	GameSaveManager(SaveHandler& handler);
	~GameSaveManager();

	// Load inventory items from save data
	// Called during game initialization to restore hero inventory
	std::map<int, std::unique_ptr<Item>> loadInventoryItems(
		std::vector<std::pair<int, int>> items);

	// Save current game state
	// isCheckpointSave: if true, saves from checkpoint pos/hp; if false, saves current pos/hp
	void saveGame(
		Hero* hero,
		Map* currentMap,
		Bloodstain* bloodstain,
		std::vector<int>& openDoorsIds,
		std::vector<int>& defeatedBossesIds,
		bool isCheckpointSave = false);

	// Load game state from save file
	// Returns true on success, false on error
	bool loadGame();

	// Getters for loaded save data (called after loadGame)
	int getHeroHp();
	int getHeroX();
	int getHeroY();
	int getEssence();
	std::string getCurrentMapFile();
	std::vector<std::pair<int, int>> getItems();
	std::vector<int> getOpenDoorsIds();
	std::vector<int> getDefeatedBossesIds();
	BloodstainInfo getBloodstainInfo();

private:
	SaveHandler& saveHandler;
};

#endif
