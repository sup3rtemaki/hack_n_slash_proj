#include "item.h"

#include "hero.h"

const int Item::HONEYDEW_POTION_ID = 0;

Item::Item(AnimationSet* animSet) {
	this->animSet = animSet;
	solid = false;
	type = "item";
	collisionBoxW = 6;
	collisionBoxH = 6;
	collisionBoxYOffset = -3;

	//updateCollisionBox();
	//changeAnimation();
	//changeAnimation(0, false, "itemOnGround");
}

Item::~Item(){

}

void Item::update() {
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

void Item::changeAnimation(int newState, bool resetFrameToBeginning) {
	currentAnim = animSet->getAnimation("item");
	currentFrame = currentAnim->getFrame(0);
}

// TODO: Teste, tornar esse método virtual
void Item::applyEffect(LivingEntity* heroEntity) {
	if (dynamic_cast<Hero*>((heroEntity)) != nullptr) {
		Hero* h = (Hero*)heroEntity;
		h->hp += 3;
	}
}
