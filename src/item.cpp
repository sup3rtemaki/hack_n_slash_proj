#include "item.h"

#include "hero.h"

const int Item::HONEYDEW_POTION_ID = 0;

const string Item::ITEM_CONSUMABLE = "consumable";
const string Item::ITEM_PASSIVE = "passive";
const string Item::ITEM_KEY = "key";

Item::Item(AnimationSet* animSet) {
	this->animSet = animSet;
	solid = false;
	type = "item";
	collisionBoxW = 8;
	collisionBoxH = 8;
	collisionBoxYOffset = -4;

	//updateCollisionBox();
	//changeAnimation();
	//changeAnimation(0, false, "itemOnGround");
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

// TODO: Teste, tornar esse método virtual
//void Item::applyEffect(LivingEntity* heroEntity) {
//	if (dynamic_cast<Hero*>((heroEntity)) != nullptr) {
//		Hero* h = (Hero*)heroEntity;
//		h->hp += 3;
//	}
//}
