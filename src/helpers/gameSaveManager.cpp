#include "helpers/gameSaveManager.h"
#include "helpers/saveHandler.h"
#include "hero.h"
#include "map.h"
#include "bloodstain.h"
#include "item/itemsHub.h"
#include "item/stone.h"
#include "item/key.h"
#include <iostream>

using namespace std;

GameSaveManager::GameSaveManager(SaveHandler& handler) : saveHandler(handler) {
}

GameSaveManager::~GameSaveManager() {
}

map<int, unique_ptr<Item>> GameSaveManager::loadInventoryItems(
	vector<pair<int, int>> items) {

	map<int, unique_ptr<Item>> loadedItems;

	if (!items.empty()) {
		for (auto item : items) {
			unique_ptr<Item> loadItem;

			switch (item.first) {
			case Item::HONEYDEW_POTION_ID:
				loadItem = make_unique<HoneydewPotion>(
					false, item.second);
				loadItem->active = false;
				break;
			case Item::GREEN_BERRY_ID:
				loadItem = make_unique<GreenBerry>(
					false, item.second);
				loadItem->active = false;
				break;
			case Item::STONE_ID:
				loadItem = make_unique<Stone>(
					false, item.second);
				loadItem->active = false;
				break;
			case Item::COMMON_KEY_ID:
				loadItem = make_unique<Key>(
					false, item.second);
				loadItem->active = false;
				break;
			default:
				cout << "Item " << item.first << " nao mapeado\n";
				continue;
			}

			if (loadItem != nullptr) {
				int itemId = loadItem->id;
				loadedItems.emplace(itemId, move(loadItem));
			}
		}
	}

	return loadedItems;
}

void GameSaveManager::saveGame(
	Hero* hero,
	Map* currentMap,
	Bloodstain* bloodstain,
	vector<int>& openDoorsIds,
	vector<int>& defeatedBossesIds,
	bool isCheckpointSave) {

	cout << "Saving..." << endl;

	float hp;
	int x, y;
	string mapFile;

	if (isCheckpointSave) {
		hp = hero->hpMax;
		x = hero->lastCheckpointPos.x;
		y = hero->lastCheckpointPos.y;
		mapFile = hero->lastCheckpointMapFile;
	}
	else {
		hp = hero->hp;
		x = hero->x;
		y = hero->y;
		mapFile = currentMap->file;
	}

	vector<pair<int, int>> inventory;
	for (auto& [id, item] : hero->inventory) {
		inventory.push_back(make_pair(id, item->quantity));
	}

	saveHandler.save(
		hp,
		x,
		y,
		hero->essence,
		mapFile,
		inventory,
		openDoorsIds,
		defeatedBossesIds,
		bloodstain->x,
		bloodstain->y,
		bloodstain->essence,
		bloodstain->mapName);
}

bool GameSaveManager::loadGame() {
	if (!saveHandler.load()) {
		cerr << "Erro ao carregar savefile" << endl;
		return false;
	}
	return true;
}

int GameSaveManager::getHeroHp() {
	return saveHandler.getHeroHp();
}

int GameSaveManager::getHeroX() {
	return saveHandler.getHeroX();
}

int GameSaveManager::getHeroY() {
	return saveHandler.getHeroY();
}

int GameSaveManager::getEssence() {
	return saveHandler.getEssence();
}

string GameSaveManager::getCurrentMapFile() {
	return saveHandler.getCurrentMapFile();
}

vector<pair<int, int>> GameSaveManager::getItems() {
	return saveHandler.getItems();
}

vector<int> GameSaveManager::getOpenDoorsIds() {
	return saveHandler.getOpenDoorsIds();
}

vector<int> GameSaveManager::getDefeatedBossesIds() {
	return saveHandler.getDefeatedBossesIds();
}

BloodstainInfo GameSaveManager::getBloodstainInfo() {
	return saveHandler.getBloodstainInfo();
}
