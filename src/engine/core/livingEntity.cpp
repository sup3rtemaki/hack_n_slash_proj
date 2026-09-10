#include "livingEntity.h"

const float LivingEntity::TIME_TICK = 60.0f;

void LivingEntity::updateHitBox() {
	// assume damage is 0
	damage = 0;

	GroupBox* hitBoxes = (GroupBox*)GroupBuilder::findGroupByName("hitBox", currentFrame->frameData);
	if (hitBoxes != nullptr && hitBoxes->numberOfDataInGroup() > 0) {
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
		invincibleTimer -= deltaTime;
	}
}

void LivingEntity::draw(const RenderContext& context) {
	if (currentFrame != NULL && active) {
		if (invincibleTimer > 0 && animSet->whiteSpriteSheet != NULL) {
			currentFrame->Draw(
				animSet->whiteSpriteSheet,
				x - (context.camera.x * 1.f),
				y - (context.camera.y * 1.f),
				context);
		}
		else {
			currentFrame->Draw(animSet->spriteSheet,
				x - (context.camera.x * 1.f),
				y - (context.camera.y * 1.f),
				context);
		}
	}

	//draw collision box
	if (solid && context.debugging) {
		// Criar rects tempor�rios ajustados pela c�mera
		SDL_Rect screenLastCollisionBox = {
			lastCollisionBox.x - context.camera.x,
			lastCollisionBox.y - context.camera.y,
			lastCollisionBox.w,
			lastCollisionBox.h
		};

		SDL_Rect screenCollisionBox = {
			collisionBox.x - context.camera.x,
			collisionBox.y - context.camera.y,
			collisionBox.w,
			collisionBox.h
		};

		SDL_Rect screenHitBox = {
			hitBox.x - context.camera.x,
			hitBox.y - context.camera.y,
			hitBox.w,
			hitBox.h
		};

		// Desenhar com os rects ajustados
		SDL_SetRenderDrawColor(context.renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawRect(context.renderer, &screenLastCollisionBox);

		SDL_SetRenderDrawColor(context.renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawRect(context.renderer, &screenCollisionBox);

		SDL_SetRenderDrawColor(context.renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawRect(context.renderer, &screenHitBox);

		SDL_SetRenderDrawColor(context.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(context.renderer,
			x - context.camera.x,
			y + (collisionBoxYOffset / 2) - context.camera.y);

		SDL_SetRenderDrawColor(context.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(context.renderer,
			x - context.camera.x,
			y - context.camera.y);
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
