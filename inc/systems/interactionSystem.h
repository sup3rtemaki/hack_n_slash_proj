#pragma once

#include <vector>
#include <functional>

class Entity;
class Hero;
class ActionMessageUi;

class InteractionSystem {
public:
	InteractionSystem(
		Hero* hero,
		ActionMessageUi* actionMessageUi,
		std::vector<int>& openDoorsIds,
		bool& mustRemoveAllEnemies,
		bool& mustSpawnEnemies
	);

	void processEntity(Entity* entity);

private:
	Hero* hero;
	ActionMessageUi* actionMessageUi;
	std::vector<int>& openDoorsIds;
	bool& mustRemoveAllEnemies;
	bool& mustSpawnEnemies;

	void checkAndHandleEnemyLoot(Entity* entity);
	void checkAndHandleNearItem(Entity* entity);
	void checkAndHandleNearDoor(Entity* entity);
	void checkAndHandleNearCheckpoint(Entity* entity);
	void checkAndHandleNearBloodstain(Entity* entity);
	
	bool isLivingEntityDead(Entity* entity);

	// Callbacks - will be set by Game
	std::function<void(int, int, int, int)> spawnItemCallback;
	std::function<void(int)> saveCheckpointCallback;

public:
	void setSpawnItemCallback(std::function<void(int, int, int, int)> callback) {
		spawnItemCallback = callback;
	}

	void setSaveCheckpointCallback(std::function<void(int)> callback) {
		saveCheckpointCallback = callback;
	}
};
