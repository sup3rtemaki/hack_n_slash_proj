#pragma once

#include <functional>
#include <list>

class Map;
class Entity;

class MapStateSystem {
public:
	MapStateSystem() = default;

	void persistPickedMapItems(Map* currentMap);
	void inactivateCurrentMapItems(std::list<Entity*>& entities);
	void removeAllEnemiesInMap(
		std::list<Entity*>& entities,
		std::list<Entity*>& currentMapEnemies,
		std::list<int>& deadEnemiesIds);
	void saveCheckpointActivatedState(Map* currentMap, int checkpointId);

	void setSyncRegistryCallback(std::function<void()> callback) {
		syncRegistryCallback = callback;
	}

private:
	std::function<void()> syncRegistryCallback;
};
