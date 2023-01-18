#include "item.h"

//const int Item::HONEYDEW_POTION_ID = 0;

const string Item::ITEM_CONSUMABLE = "consumable";
const string Item::ITEM_PASSIVE = "passive";
const string Item::ITEM_KEY = "key";

Item::Item(SDL_Texture* image, AnimationSet* animSet) {
	this->image = image;
	this->animSet = animSet;
	solid = false;
	isNearHero = false;
	type = "item";
	collisionBoxW = 8;
	collisionBoxH = 8;
	collisionBoxYOffset = -4;

}

Item::~Item(){
}

void Item::update() {
	updateCollisionBox();

	if (isOnGround != solid) {
		if (isOnGround) {
			changeAnimation(0, true);
		}
	}

	if (currentFrame == NULL || currentAnim == NULL) {
		return;
	}

	frameTimer += TimeController::timeController.dT;

	if (frameTimer >= currentFrame->duration) {
		currentFrame = currentAnim->getNextFrame(currentFrame);
		frameTimer = 0;
	}
}

void Item::changeAnimation(int newState, bool resetFrameToBeginning, string animName) {
	if (!animName.empty()) {
		currentAnim = animSet->getAnimation(animName);
	}
	currentFrame = currentAnim->getFrame(0);
}
