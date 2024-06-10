#include "livingEntity.h"

const float LivingEntity::TIME_TICK = 60.0f;

void LivingEntity::updateHitBox() {
	// assume damage is 0
	damage = 0;

	GroupBox* hitBoxes = (GroupBox*)GroupBuilder::findGroupByName("hitBox", currentFrame->frameData);
	if (hitBoxes != NULL && hitBoxes->numberOfDataInGroup() > 0) {
		//update hitbox
		SDL_Rect hb = hitBoxes->data.front();
		hitBox.x = x - currentFrame->offset.x + hb.x;
		hitBox.y = y - currentFrame->offset.y + hb.y;
		hitBox.w = hb.w;
		hitBox.h = hb.h;

		//update damage
		GroupNumber* damages = (GroupNumber*)GroupBuilder::findGroupByName("damage", currentFrame->frameData);
		if (damages != NULL && damages->numberOfDataInGroup() > 0) {
			damage = damages->data.front();
		}
	}
}

void LivingEntity::updateInvincibleTimer() {
	if (invincibleTimer > 0) {
		invincibleTimer -= TimeController::timeController.dT;
	}
}

void LivingEntity::draw() {
	if (currentFrame != NULL && active) {
		if (invincibleTimer > 0 && animSet->whiteSpriteSheet != NULL) {
			currentFrame->Draw(
				animSet->whiteSpriteSheet,
				x - (Globals::camera.x * 1.f),
				y - (Globals::camera.y * 1.f));
		}
		else {
			currentFrame->Draw(animSet->spriteSheet,
				x - (Globals::camera.x * 1.f),
				y - (Globals::camera.y * 1.f));
		}
		
	}
	//draw collision box
	if (solid && Globals::debugging) {
		SDL_SetRenderDrawColor(Globals::renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawRect(Globals::renderer, &lastCollisionBox);

		SDL_SetRenderDrawColor(Globals::renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawRect(Globals::renderer, &collisionBox);

		SDL_SetRenderDrawColor(Globals::renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawRect(Globals::renderer, &hitBox);

		SDL_SetRenderDrawColor(Globals::renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(Globals::renderer, x, y + (collisionBoxYOffset / 2));

		SDL_SetRenderDrawColor(Globals::renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(Globals::renderer, x, y);
	}
}

void LivingEntity::changeAnimation(int newState, bool resetFrameToBeginning, string animName){
}

void LivingEntity::saveDeadEnemiesIds(list<Entity*> entityList, list<int>& enemiesIdList){
	for (list<Entity*>::iterator enemyEntity = entityList.begin(); enemyEntity != entityList.end(); enemyEntity++) {
		//Checks if Entity is a LivingEntity
		if (dynamic_cast<LivingEntity*>((*enemyEntity)) != nullptr) {
			LivingEntity* livingEntity = (LivingEntity*)(*enemyEntity);
			if (livingEntity->hp <= 0) {
				enemiesIdList.push_back(livingEntity->enemyId);
			}
		}
	}
}
