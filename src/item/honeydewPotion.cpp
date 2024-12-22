#include "item/honeydewPotion.h"

HoneydewPotion::HoneydewPotion(AnimationSet* animSet, bool isOnGround, int quant) {
	this->animSet = animSet;
	id = HONEYDEW_POTION_ID;
	quantity = quant;
	name = "Honeydew Potion";
	itemType = ITEM_CONSUMABLE;
	type = "cItem";
	description = "Sweet honeydew harvested from aphids";
	this->isOnGround = isOnGround;

	string resPath = getResourcePath() + Item::ITEMS_IMAGES_FOLDER;
	this->image = loadTexture(resPath + "honeyDewPotion.png", Globals::renderer);

	collisionBoxW = 8;
	collisionBoxH = 8;
	collisionBoxYOffset = -4;

	updateCollisionBox();
	changeAnimation(0, false, "idle");
}

HoneydewPotion::~HoneydewPotion(){
}

void HoneydewPotion::update() {
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

void HoneydewPotion::changeAnimation(int newState, bool resetFrameToBeginning, string animName) {
	if (!animName.empty()) {
		currentAnim = animSet->getAnimation(animName);
	}
	else {
		currentAnim = animSet->getAnimation("idle");
	}
	//currentAnim = animSet->getAnimation("idle");
	currentFrame = currentAnim->getFrame(0);
}

void HoneydewPotion::applyEffect(LivingEntity* heroEntity){
	heroEntity->hp += 5;
	/*if (dynamic_cast<Hero*>((heroEntity)) != nullptr) {
		Hero* h = (Hero*)heroEntity;
		h->hp += 5;
		cout << "Aplicou efeito: " << h->hp << "\n";
	}*/
}
