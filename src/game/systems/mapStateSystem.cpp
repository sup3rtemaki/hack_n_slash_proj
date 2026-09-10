#include "mapStateSystem.h"

#include "map.h"
#include "entity.h"
#include "resourceConfig.h"
#include "jsonFileStore.h"
#include "item.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

void MapStateSystem::persistPickedMapItems(Map* currentMap) {
	if (currentMap == nullptr) {
		return;
	}

	const std::string itemsMapFilePath = getResourcePath() + ResourcePaths::MAPS + currentMap->file;
	json mapFile;
	if (JsonFileStore::readJsonFile(itemsMapFilePath, mapFile) != JsonFileResult::Success) {
		return;
	}

	for (auto const& item : currentMap->itemsInMap) {
		if (!item.first) {
			continue;
		}

		for (auto& layer : mapFile["layers"]) {
			if (layer["name"] != "Items") {
				continue;
			}

			for (auto& object : layer["objects"]) {
				if ((int)object["x"] != std::get<1>(item.second) ||
					(int)object["y"] != std::get<2>(item.second)) {
					continue;
				}

				bool matchingItem = false;
				for (auto& property : object["properties"]) {
					if (property["name"] == "itemId" &&
						property["value"] == std::get<0>(item.second)) {
						matchingItem = true;
						break;
					}
				}

				if (!matchingItem) {
					continue;
				}

				for (auto& property : object["properties"]) {
					if (property["name"] == "isPicked") {
						property["value"] = true;
					}
				}
			}
		}
	}

	JsonFileStore::writeJsonFile(itemsMapFilePath, mapFile);
}

void MapStateSystem::inactivateCurrentMapItems(std::list<Entity*>& entities) {
	for (auto entity : entities) {
		if (dynamic_cast<Item*>(entity) != nullptr) {
			entity->active = false;
		}
	}

	if (syncRegistryCallback) {
		syncRegistryCallback();
	}
}

void MapStateSystem::removeAllEnemiesInMap(
	std::list<Entity*>& entities,
	std::list<Entity*>& currentMapEnemies,
	std::list<int>& deadEnemiesIds) {
	for (auto enemy : currentMapEnemies) {
		enemy->active = false;
	}
	Entity::removeInactiveEntitiesFromList(&entities, false);
	if (syncRegistryCallback) {
		syncRegistryCallback();
	}
	Entity::removeInactiveEntitiesFromList(&currentMapEnemies, true);
	currentMapEnemies.clear();
	deadEnemiesIds.clear();
}

void MapStateSystem::saveCheckpointActivatedState(Map* currentMap, int checkpointId) {
	if (currentMap == nullptr) {
		return;
	}

	const std::string mapFilePath = getResourcePath() + ResourcePaths::MAPS + currentMap->file;
	json mapFile;
	if (JsonFileStore::readJsonFile(mapFilePath, mapFile) != JsonFileResult::Success) {
		return;
	}

	for (auto& layersIt : mapFile["layers"]) {
		std::string name = layersIt["name"];
		if (name == "Checkpoints") {
			for (auto& object : layersIt["objects"]) {
				for (auto& prop : object["properties"]) {
					if (prop["name"] == "checkpointId" && prop["value"] == checkpointId) {
						for (auto& prop2 : object["properties"]) {
							if (prop2["name"] == "isActive") {
								prop2["value"].clear();
								prop2["value"] = true;
								JsonFileStore::writeJsonFile(mapFilePath, mapFile);
								return;
							}
						}
					}
				}
			}
		}
	}
}
