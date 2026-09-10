#include "mapFlowSystem.h"

#include "map.h"
#include "entity.h"
#include "hero.h"
#include "bloodstain.h"
#include "resourceConfig.h"
#include "mapPopulationSystem.h"

#include <string>

MapFlowSystem::MapFlowSystem(
	std::unique_ptr<tson::Map>& tiledMap,
	Map*& currentMap,
	std::list<Entity*>& entities,
	std::list<Entity*>& fogWalls,
	std::list<Entity*>& currentMapEnemies,
	Hero*& hero,
	Bloodstain*& bloodstain,
	std::vector<int>& openDoorsIds,
	LivingEntity*& currentBoss,
	MapPopulationSystem* mapPopulationSystem)
	: tiledMap(tiledMap),
	currentMap(currentMap),
	entities(entities),
	fogWalls(fogWalls),
	currentMapEnemies(currentMapEnemies),
	hero(hero),
	bloodstain(bloodstain),
	openDoorsIds(openDoorsIds),
	currentBoss(currentBoss),
	mapPopulationSystem(mapPopulationSystem) {
}

void MapFlowSystem::applyMapTransition(const std::string& nextMapFile) {
	currentMap->file = nextMapFile;
	hero->attackBuffer.clear();

	if (loadTiledMapCallback) {
		loadTiledMapCallback(getResourcePath() + ResourcePaths::MAPS + currentMap->file);
	}

	for (auto enemy : currentMapEnemies) {
		enemy->active = false;
	}

	for (auto fogWall : fogWalls) {
		fogWall->active = false;
	}
	fogWalls.clear();

	for (auto entity : entities) {
		if (entity->type == "wall" ||
			entity->type == "door" ||
			entity->type == "checkpoint" ||
			entity->type == "boss") {
			entity->active = false;
		}
	}
	if (syncRegistryCallback) {
		syncRegistryCallback();
	}

	currentBoss = nullptr;
	hero->currentMap = currentMap;
	if (mapPopulationSystem != nullptr) {
		mapPopulationSystem->spawnItemsFromCurrentMap();
		mapPopulationSystem->buildWalls();
		mapPopulationSystem->buildWaypoints();
		mapPopulationSystem->buildDoors();
		mapPopulationSystem->spawnCheckpoints();
	}
	if (bloodstain != nullptr && bloodstain->isLive && currentMap->file == bloodstain->mapName) {
		bloodstain->create();
	}
	else if (bloodstain != nullptr) {
		bloodstain->destroy();
	}
	openDoorsIds.clear();
}
