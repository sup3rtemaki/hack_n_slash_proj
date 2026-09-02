#include "systems/interactionSystem.h"

#include "entity.h"
#include "hero.h"
#include "item.h"
#include "item/itemsHub.h"
#include "npcs/door.h"
#include "checkpoint.h"
#include "bloodstain.h"
#include "livingEntity.h"
#include "entities/lootDropSource.h"
#include "ui/actionMessageUi.h"

InteractionSystem::InteractionSystem(
	Hero* hero,
	ActionMessageUi* actionMessageUi,
	std::vector<int>& openDoorsIds,
	bool& mustRemoveAllEnemies,
	bool& mustSpawnEnemies) : hero(hero),
	actionMessageUi(actionMessageUi),
	openDoorsIds(openDoorsIds),
	mustRemoveAllEnemies(mustRemoveAllEnemies),
	mustSpawnEnemies(mustSpawnEnemies)
{
}

void InteractionSystem::processEntity(Entity* entity) {
	checkAndHandleEnemyLoot(entity);
	checkAndHandleNearItem(entity);
	checkAndHandleNearDoor(entity);
	checkAndHandleNearBloodstain(entity);
	checkAndHandleNearCheckpoint(entity);
}

void InteractionSystem::checkAndHandleEnemyLoot(Entity* entity) {
	if (entity->type != "enemy") return;

	LootDropSource* lootSource = dynamic_cast<LootDropSource*>(entity);
	if (lootSource == nullptr) return;

	PendingItemDrop itemDrop;
	if (lootSource->takePendingItemDrop(itemDrop)) {
		if (spawnItemCallback != nullptr) {
			spawnItemCallback(itemDrop.itemId, itemDrop.quantity, itemDrop.x, itemDrop.y);
		}
	}

	if (isLivingEntityDead(entity) && !lootSource->hasDroppedEssence()) {
		hero->addEssence(lootSource->getEssence());
		lootSource->markEssenceDropped();
	}
}

void InteractionSystem::checkAndHandleNearItem(Entity* entity) {
	if (dynamic_cast<Item*>(entity) != nullptr) {
		Item* i = (Item*)entity;
		if (i->isOnGround &&
			(Entity::distanceBetweenTwoPoints(hero->x, hero->y + (hero->collisionBoxYOffset / 2), i->x, i->y) < 40.0)) {
			if (!i->isNearHero) {
				i->isNearHero = true;
				hero->nearItems.push_back(i);
			}
		}
		else {
			i->isNearHero = false;
			hero->nearItems.remove(i);
		}
	}
}

void InteractionSystem::checkAndHandleNearDoor(Entity* entity) {
	if (dynamic_cast<Door*>(entity) != nullptr) {
		Door* d = (Door*)entity;
		if (d->isClosed &&
			(Entity::distanceBetweenTwoPoints(
				hero->x, hero->y + (hero->collisionBoxYOffset / 2), d->x + 32, d->y) < 60.0)) {
			hero->nearestDoor = d;
			if (!actionMessageUi->isUiLocked()) {
				actionMessageUi->setMessage("Open door");
			}
		}
		else {
			hero->nearestDoor = nullptr;
			actionMessageUi->unlock();

			if (!d->isClosed) {
				if (std::find(openDoorsIds.begin(), openDoorsIds.end(), d->id) == openDoorsIds.end()) {
					openDoorsIds.push_back(d->id);
				}
			}
		}
	}
}

void InteractionSystem::checkAndHandleNearCheckpoint(Entity* entity) {
	if (dynamic_cast<Checkpoint*>(entity) != nullptr) {
		Checkpoint* cp = (Checkpoint*)entity;
		if (Entity::distanceBetweenTwoPoints(
			hero->x, hero->y + (hero->collisionBoxYOffset / 2.f), cp->x + 32.f, cp->y) < 60.0) {
			hero->nearestCheckpoint = cp;

			cp->isActivated ?
				actionMessageUi->unlock() :
				actionMessageUi->setMessage("Activate checkpoint");
		}
		else {
			hero->nearestCheckpoint = nullptr;
			actionMessageUi->unlock();
		}

		if (hero->isCheckpointActivatedFlag) {
			// saves checkpoint activated status
			hero->isCheckpointActivatedFlag = false;
			if (saveCheckpointCallback != nullptr) {
				saveCheckpointCallback(cp->id);
			}
			actionMessageUi->setMessage("Checkpoint active!");
			actionMessageUi->setTimer(3.f);
		}

		if (hero->state == (int)HERO_STATE::RESTING && hero->isRested) {
			hero->isRested = false;
			mustRemoveAllEnemies = true;
			mustSpawnEnemies = true;
		}
	}
}

void InteractionSystem::checkAndHandleNearBloodstain(Entity* entity) {
	if (dynamic_cast<Bloodstain*>(entity) != nullptr) {
		Bloodstain* bloodstain = (Bloodstain*)entity;
		if (bloodstain->isLive) {
			if (Entity::distanceBetweenTwoPoints(
				hero->x, hero->y + (hero->collisionBoxYOffset / 2.f), bloodstain->x + 32.f, bloodstain->y) < 60.0) {
				hero->nearestBloodstain = bloodstain;
				actionMessageUi->setMessage("Recover lost essence");
			}
			else {
				hero->nearestBloodstain = nullptr;
			}
		}
	}
}

bool InteractionSystem::isLivingEntityDead(Entity* entity) {
	if (entity->active &&
		dynamic_cast<LivingEntity*>(entity) != nullptr &&
		((LivingEntity*)entity)->hp <= 0) {
		return true;
	}
	return false;
}
