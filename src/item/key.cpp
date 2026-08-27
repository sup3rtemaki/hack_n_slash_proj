#include "item/key.h"

#include "hero.h"
#include "resourceConfig.h"

Key::Key(bool isOnGround, int quant) {
	list<DataGroupType> dataGroupTypes;
	DataGroupType colBoxType; colBoxType.groupName = "collisionBox"; colBoxType.dataType = DataGroupType::DATATYPE_BOX;
	DataGroupType hitBoxType; hitBoxType.groupName = "hitBox"; hitBoxType.dataType = DataGroupType::DATATYPE_BOX;
	DataGroupType dmgType; dmgType.groupName = "damage"; dmgType.dataType = DataGroupType::DATATYPE_NUMBER;
	dataGroupTypes.push_back(colBoxType);
	dataGroupTypes.push_back(hitBoxType);
	dataGroupTypes.push_back(dmgType);

	localAnimSet = std::make_unique<AnimationSet>();
	localAnimSet->loadAnimationSet(ResourcePaths::ANIMATIONS + "groundConsumableItem.fdset", dataGroupTypes, Globals::renderer);
	this->animSet = localAnimSet.get();
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

	frameTimer += deltaTime;

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

	frameTimer += deltaTime;

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
