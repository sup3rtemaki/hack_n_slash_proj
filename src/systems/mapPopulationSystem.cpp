#include "systems/mapPopulationSystem.h"
#include "map.h"
#include "entity.h"
#include "wall.h"
#include "hero.h"
#include "ui/hpBar.h"
#include "npcs/door.h"
#include "checkpoint.h"
#include "termiteMiner.h"
#include "npcs/bosses/smallBrownSpider.h"
#include "soundManager.h"
#include "tileson/tileson.hpp"
#include "livingEntity.h"

#include <iostream>
#include <any>
#include <algorithm>

using namespace std;

MapPopulationSystem::MapPopulationSystem(
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
)
	: tiledMap(tiledMap),
	currentMap(currentMap),
	entities(entities),
	walls(walls),
	fogWalls(fogWalls),
	currentMapEnemies(currentMapEnemies),
	hero(hero),
	currentBoss(currentBoss),
	gui(gui),
	openDoorsIds(openDoorsIds),
	defeatedBossesIds(defeatedBossesIds),
	deadEnemiesIds(deadEnemiesIds),
	bossHpBar(bossHpBar)
{
}

void MapPopulationSystem::buildDoors() {
	auto tMap = tiledMap.get();
	if (tMap == nullptr) {
		cout << "Mapa nulo" << endl;
		return;
	}

	auto layer = tMap->getLayer("Doors");
	if (layer == nullptr) return;

	int idCounter = 0;
	for (auto& [pos, tileObject] : layer->getTileObjects()) {
		auto animPrefixProp = tileObject.getTile()->getProp("animPrefix");
		auto isLockedProp = tileObject.getTile()->getProp("isLocked");
		auto animSetProp = tileObject.getTile()->getProp("animSet");

		if (animPrefixProp == nullptr ||
			isLockedProp == nullptr ||
			animSetProp == nullptr) continue;

		string animName = std::any_cast<string>(animSetProp->getValue());

		int doorId = 1000 + (currentMap->id * 10) + idCounter;

		if (animName == "doubleDoors") {
			bool isDoorClosed = true;

			if (!openDoorsIds.empty()) {
				for (int id : openDoorsIds) {
					if (id == doorId) isDoorClosed = false;
				}
			}

			Door* door = new Door(
				doorId,
				std::any_cast<string>(animPrefixProp->getValue()),
				isDoorClosed,
				tileObject.getTile()->getPosition(pos).x,
				tileObject.getTile()->getPosition(pos).y,
				64,
				64,
				-32);
			door->setSoundManager(&SoundManager::soundManager);
			door->isLocked = std::any_cast<bool>(isLockedProp->getValue()),
			entities.push_back(door);
			if (syncRegistryCallback) syncRegistryCallback();
		}

		idCounter++;
	}
}

void MapPopulationSystem::buildWalls() {
	auto tMap = tiledMap.get();
	if (tMap == nullptr) {
		cout << "Mapa nulo" << endl;
		return;
	}

	for (auto layer : tMap->getLayers()) {
		if (&layer == nullptr) return;

		for (auto& [pos, tileObject] : layer.getTileObjects()) {
			for (auto it : tileObject.getTile()->getObjectgroup().getObjectsByName("Wall")) {
				Wall* newWall = new Wall(it.getSize().x, it.getSize().y, 0);
				newWall->setSoundManager(&SoundManager::soundManager);
				newWall->x = tileObject.getPosition().x + 16;
				newWall->y = tileObject.getPosition().y;
				walls.push_back(newWall);
				entities.push_back(newWall);
				if (syncRegistryCallback) syncRegistryCallback();
			}
		}
	}
}

void MapPopulationSystem::buildWaypoints() {
	auto tMap = tiledMap.get();
	if (tMap == nullptr) {
		cout << "Mapa nulo" << endl;
		return;
	}

	auto layer = tMap->getLayer("Waypoints");
	for (auto object : layer->getObjects()) {
		Map::Waypoint waypoint;
		waypoint.nextMapFile = std::any_cast<string>(object.getProp("nextMapFileName")->getValue());
		waypoint.xDestination = std::any_cast<int>(object.getProp("xDest")->getValue());
		waypoint.yDestination = std::any_cast<int>(object.getProp("yDest")->getValue());
		waypoint.waypointRect.x = object.getPosition().x;
		waypoint.waypointRect.y = object.getPosition().y;
		waypoint.waypointRect.w = object.getSize().x;
		waypoint.waypointRect.h = object.getSize().y;

		currentMap->currentMapWaypoints.push_back(waypoint);

		if (isBossMapCallback && isBossMapCallback()) {
			Wall* fogWall = new Wall(
				waypoint.waypointRect.w + 2,
				waypoint.waypointRect.h + 2,
				0);

			fogWall->x = waypoint.waypointRect.x - 1;
			fogWall->y = waypoint.waypointRect.y - 1;
			fogWalls.push_back(fogWall);
			entities.push_back(fogWall);
			if (syncRegistryCallback) syncRegistryCallback();
		}
	}
}

void MapPopulationSystem::spawnEnemies() {
	auto tMap = tiledMap.get();
	if (tMap == nullptr) {
		cout << "Mapa nulo" << endl;
		return;
	}

	auto layer = tMap->getLayer("EnemiesSpawn");
	if (layer == nullptr) {
		return;
	}

	int enemyPosX, enemyPosY;
	int uniqueId;
	int idCounter = 0;
	for (auto object : layer->getObjects()) {
		int enemyId = std::any_cast<int>(object.getProp("enemyId")->getValue());
		uniqueId = (enemyId * 10) + idCounter;
		enemyPosX = object.getPosition().x;
		enemyPosY = object.getPosition().y;
		switch (enemyId) {
		case 0: // Glob
			if (deadEnemiesIds.empty() ||
				std::find(deadEnemiesIds.begin(), deadEnemiesIds.end(), uniqueId) == deadEnemiesIds.end()) {
					TermiteMiner* enemy = new TermiteMiner();
					enemy->setSoundManager(&SoundManager::soundManager);
					enemy->x = enemyPosX;
					enemy->y = enemyPosY;
					enemy->invincibleTimer = 0.1;
					enemy->enemyId = uniqueId;
					currentMapEnemies.push_back(enemy);
					entities.push_back(enemy);
					if (syncRegistryCallback) syncRegistryCallback();
			}
			break;
		case 1: // Termite
			if (deadEnemiesIds.empty() ||
				std::find(deadEnemiesIds.begin(), deadEnemiesIds.end(), uniqueId) == deadEnemiesIds.end()) {
					TermiteMiner* enemy = new TermiteMiner();
					enemy->setSoundManager(&SoundManager::soundManager);
					enemy->x = enemyPosX;
					enemy->y = enemyPosY;
					enemy->invincibleTimer = 0.1;
					enemy->enemyId = uniqueId;
					currentMapEnemies.push_back(enemy);
					entities.push_back(enemy);
					if (syncRegistryCallback) syncRegistryCallback();
			}
			break;
		default:
			break;
		}

		idCounter++;
	}
}

void MapPopulationSystem::spawnBoss() {
	auto tMap = tiledMap.get();
	if (tMap == nullptr) {
		cout << "Mapa nulo" << endl;
		return;
	}

	auto layer = tMap->getLayer("BossSpawn");
	if (layer == nullptr) return;

	for (auto object : layer->getObjects()) {
		int bossId = std::any_cast<int>(object.getProp("bossId")->getValue());
		
		// Check if this specific boss was already defeated
		bool bossDefeated = false;
		for (int i : defeatedBossesIds) {
			if (i == bossId) {
				bossDefeated = true;
				break;
			}
		}
		if (bossDefeated) continue;  // Skip this boss, continue to next object

		int bossPosX = object.getPosition().x;
		int bossPosY = object.getPosition().y;
		switch (bossId) {
			case 990001: // Small Brown Spider
				currentBoss = new SmallBrownSpider();
				currentBoss->setSoundManager(&SoundManager::soundManager);
				currentBoss->x = bossPosX;
				currentBoss->y = bossPosY;
				currentBoss->id = bossId;
				currentBoss->invincibleTimer = 0.1;
				entities.push_back(currentBoss);
				if (syncRegistryCallback) syncRegistryCallback();
				bossHpBar = new HPBar(currentBoss, BarType::BOSS_HEALTH_BAR);
				gui.push_back(bossHpBar);
				break;
			default:
				break;
		}
	}
}

void MapPopulationSystem::spawnCheckpoints() {
	auto tMap = tiledMap.get();
	if (tMap == nullptr) {
		cout << "Mapa nulo" << endl;
		return;
	}

	auto layer = tMap->getLayer("Checkpoints");
	if (layer == nullptr) return;

	for (auto object : layer->getObjects()) {
		bool isActive = std::any_cast<bool>(object.getProp("isActive")->getValue());
		int cpId = std::any_cast<int>(object.getProp("checkpointId")->getValue());
		int cpPosX = object.getPosition().x;
		int cpPosY = object.getPosition().y;

		Checkpoint* checkpoint = new Checkpoint(cpId, currentMap->file);
		checkpoint->setSoundManager(&SoundManager::soundManager);
		checkpoint->x = cpPosX;
		checkpoint->y = cpPosY;
		if (isActive) checkpoint->activate();

		entities.push_back(checkpoint);
		if (syncRegistryCallback) syncRegistryCallback();
	}
}

void MapPopulationSystem::spawnItemsFromCurrentMap() {
	auto tMap = tiledMap.get();
	if (tMap == nullptr) {
		cout << "Mapa nulo" << endl;
		return;
	}
	for (auto layer : tMap->getLayers()) {
		if (&layer == nullptr) return;

		if (layer.getType() == tson::LayerType::ObjectGroup &&
			layer.getName() == "Items") {
			bool isPicked;
			int itemId;
			int xPos;
			int yPos;
			for (auto object : layer.getObjects()) {
				for (auto prop : object.getProperties().getProperties()) {
					if (prop.second.getName() == "isPicked") {
						isPicked = std::any_cast<bool>(prop.second.getValue());
					}
					else if (prop.second.getName() == "itemId") {
						itemId = std::any_cast<int>(prop.second.getValue());
					}
				}
				xPos = object.getPosition().x;
				yPos = object.getPosition().y;
				currentMap->itemsInMap.push_back(
					std::make_pair(isPicked, std::make_tuple(itemId, xPos, yPos)));

				if (!isPicked && spawnItemCallback) {
					spawnItemCallback(itemId, 1, xPos, yPos);
				}
			}
		}
	}
}
