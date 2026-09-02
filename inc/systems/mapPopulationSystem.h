#pragma once

#include <memory>
#include <vector>
#include <list>
#include <functional>

class Map;
class Entity;
class Wall;
class Hero;
class Ui;
class HPBar;
class LivingEntity;

namespace tson {
	class Map;
}

class MapPopulationSystem {
public:
	MapPopulationSystem(
		std::unique_ptr<tson::Map>& tiledMap,
		Map* currentMap,
		std::list<Entity*>& entities,
		std::list<Entity*>& walls,
		std::list<Entity*>& fogWalls,
		std::list<Entity*>& currentMapEnemies,
		Hero* hero,
		LivingEntity*& currentBoss,
		std::list<Ui*>& gui,
		std::vector<int>& openDoorsIds,
		std::vector<int>& defeatedBossesIds,
		std::list<int>& deadEnemiesIds,
		HPBar*& bossHpBar
	);

	void buildDoors();
	void buildWalls();
	void buildWaypoints();
	void spawnEnemies();
	void spawnBoss();
	void spawnCheckpoints();
	void spawnItemsFromCurrentMap();

	// Callbacks
	void setSpawnItemCallback(std::function<void(int, int, int, int)> callback) {
		spawnItemCallback = callback;
	}

	void setSyncRegistryCallback(std::function<void()> callback) {
		syncRegistryCallback = callback;
	}

	void setIsBossMapCallback(std::function<bool()> callback) {
		isBossMapCallback = callback;
	}

private:
	std::unique_ptr<tson::Map>& tiledMap;
	Map* currentMap;
	std::list<Entity*>& entities;
	std::list<Entity*>& walls;
	std::list<Entity*>& fogWalls;
	std::list<Entity*>& currentMapEnemies;
	Hero* hero;
	LivingEntity*& currentBoss;
	std::list<Ui*>& gui;
	std::vector<int>& openDoorsIds;
	std::vector<int>& defeatedBossesIds;
	std::list<int>& deadEnemiesIds;
	HPBar*& bossHpBar;

	std::function<void(int, int, int, int)> spawnItemCallback;
	std::function<void()> syncRegistryCallback;
	std::function<bool()> isBossMapCallback;
};
