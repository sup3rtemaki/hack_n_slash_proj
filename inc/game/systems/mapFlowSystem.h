#pragma once

#include <functional>
#include <list>
#include <memory>
#include <string>
#include <vector>

class Map;
class Entity;
class Hero;
class LivingEntity;
class Bloodstain;
class MapPopulationSystem;

namespace tson {
	class Map;
}

class MapFlowSystem {
public:
	MapFlowSystem(
		std::unique_ptr<tson::Map>& tiledMap,
		Map*& currentMap,
		std::list<Entity*>& entities,
		std::list<Entity*>& fogWalls,
		std::list<Entity*>& currentMapEnemies,
		Hero*& hero,
		Bloodstain*& bloodstain,
		std::vector<int>& openDoorsIds,
		LivingEntity*& currentBoss,
		MapPopulationSystem* mapPopulationSystem
	);

	void applyMapTransition(const std::string& nextMapFile);

	void setLoadTiledMapCallback(std::function<void(const std::string&)> callback) {
		loadTiledMapCallback = callback;
	}

	void setSyncRegistryCallback(std::function<void()> callback) {
		syncRegistryCallback = callback;
	}

private:
	std::unique_ptr<tson::Map>& tiledMap;
	Map*& currentMap;
	std::list<Entity*>& entities;
	std::list<Entity*>& fogWalls;
	std::list<Entity*>& currentMapEnemies;
	Hero*& hero;
	Bloodstain*& bloodstain;
	std::vector<int>& openDoorsIds;
	LivingEntity*& currentBoss;
	MapPopulationSystem* mapPopulationSystem;

	std::function<void(const std::string&)> loadTiledMapCallback;
	std::function<void()> syncRegistryCallback;
};
