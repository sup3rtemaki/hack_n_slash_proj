#include "key.h"

#include "hero.h"

Key::Key(AnimationSet* animSet, bool isOnGround, int quant = 1) {
	this->animSet = animSet;
	quantity = quant;
	id = COMMON_KEY_ID;
	name = "Common key";
	itemType = ITEM_KEY;
	type = "kCommonKeyItem";
	description = "Common Key";
	this->isOnGround = isOnGround;

	string resPath = getResourcePath() + Item::ITEMS_IMAGES_FOLDER;
	this->image = loadTexture(resPath + "key.png", Globals::renderer);

	collisionBoxW = 8;
	collisionBoxH = 8;
	collisionBoxYOffset = -4;

	updateCollisionBox();
	changeAnimation(0, false, "idle");
}

void Key::update() {
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

void Key::changeAnimation(int newState, bool resetFrameToBeginning, string animName) {
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

void Key::applyEffect(LivingEntity* heroEntity) {
	if (dynamic_cast<Hero*>((heroEntity)) == nullptr) return;

	Hero* hero = (Hero*)(heroEntity);
	if (hero->nearestDoor == nullptr) {
		this->quantity++;
		return;
	}

	hero->openDoor();
}
