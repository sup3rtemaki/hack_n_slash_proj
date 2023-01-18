#include "greenBerry.h"

GreenBerry::GreenBerry(AnimationSet* animSet, bool isOnGround, int quant) {
	this->animSet = animSet;
	id = GREEN_BERRY_ID;
	quantity = quant;
	name = "Green Berry";
	itemType = ITEM_CONSUMABLE;
	type = "cItem";
	description = "Green berry, rich in water";
	this->isOnGround = isOnGround;

	string resPath = getResourcePath();
	this->image = loadTexture(resPath + "greenBerry.png", Globals::renderer);

	collisionBoxW = 8;
	collisionBoxH = 8;
	collisionBoxYOffset = -4;

	updateCollisionBox();
	changeAnimation(0, false, "idle");
}

GreenBerry::~GreenBerry()
{
}

void GreenBerry::update() {
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

void GreenBerry::changeAnimation(int newState, bool resetFrameToBeginning, string animName) {
	if (!animName.empty()) {
		currentAnim = animSet->getAnimation(animName);
	}
	else {
		currentAnim = animSet->getAnimation("idle");
	}
	//currentAnim = animSet->getAnimation("idle");
	currentFrame = currentAnim->getFrame(0);
}

void GreenBerry::applyEffect(LivingEntity* heroEntity) {
	heroEntity->hp += 2;
}
