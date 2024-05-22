#include "stone.h"

#include "projectiles/stoneProjectile.h"

Stone::Stone(AnimationSet* animSet, bool isOnGround, int quant) {
	this->animSet = animSet;
	id = STONE_ID;
	quantity = quant;
	name = "Throwing Stone";
	itemType = ITEM_CONSUMABLE;
	type = "cProjectileItem";
	description = "Small throwing stone, good to scare small predators";
	this->isOnGround = isOnGround;

	string resPath = getResourcePath() + Item::ITEMS_IMAGES_FOLDER;;
	this->image = loadTexture(resPath + "stoneItem.png", Globals::renderer);

	collisionBoxW = 8;
	collisionBoxH = 8;
	collisionBoxYOffset = -4;

	updateCollisionBox();
	changeAnimation(0, false, "idle");
}

void Stone::update() {
	updateCollisionBox();

	if (currentFrame == NULL || currentAnim == NULL) {
		return;
	}

	frameTimer += TimeController::timeController.dT;

	if (frameTimer >= currentFrame->duration) {
		currentFrame = currentAnim->getNextFrame(currentFrame);
		frameTimer = 0;
	}
}

void Stone::changeAnimation(int newState, bool resetFrameToBeginning, string animName) {
	updateCollisionBox();

	if (currentFrame == NULL || currentAnim == NULL) {
		return;
	}

	frameTimer += TimeController::timeController.dT;

	if (frameTimer >= currentFrame->duration) {
		currentFrame = currentAnim->getNextFrame(currentFrame);
		frameTimer = 0;
	}
}

void Stone::applyEffect(LivingEntity* heroEntity) {
	if (projectileAnimSet == nullptr) {
		cout << "AnimSet nulo!\n";
		return;
	}
	int xOffset = 0;
	int yOffset = 0;
	if (heroEntity->direction == DIR_DOWN) {
		yOffset = 30;
	}
	if (heroEntity->direction == DIR_UP) {
		yOffset = -30;
	}
	if (heroEntity->direction == DIR_LEFT) {
		xOffset = -5;
		yOffset = -20;
	}
	if (heroEntity->direction == DIR_RIGHT) {
		xOffset = 5;
		yOffset = -20;
	}
	StoneProjectile* stoneProjectile = new StoneProjectile(
		projectileAnimSet, 
		heroEntity->x + xOffset, 
		heroEntity->y + yOffset);
	stoneProjectile->angle = heroEntity->angle;
	Entity::entities.push_back(stoneProjectile);
}

