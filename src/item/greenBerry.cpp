#include "item/greenBerry.h"

const float HEAL_TIME = 1.0f;
const float HEAL_AMOUNT = 5.0f;

GreenBerry::GreenBerry(bool isOnGround, int quant) {
	list<DataGroupType> dataGroupTypes;
	DataGroupType colBoxType; colBoxType.groupName = "collisionBox"; colBoxType.dataType = DataGroupType::DATATYPE_BOX;
	DataGroupType hitBoxType; hitBoxType.groupName = "hitBox"; hitBoxType.dataType = DataGroupType::DATATYPE_BOX;
	DataGroupType dmgType; dmgType.groupName = "damage"; dmgType.dataType = DataGroupType::DATATYPE_NUMBER;
	dataGroupTypes.push_back(colBoxType);
	dataGroupTypes.push_back(hitBoxType);
	dataGroupTypes.push_back(dmgType);

	localAnimSet = std::make_unique<AnimationSet>();
	localAnimSet->loadAnimationSet("Assets\\Animations\\groundConsumableItem.fdset", dataGroupTypes);
	this->animSet = localAnimSet.get();
	id = GREEN_BERRY_ID;
	quantity = quant;
	name = "Green Berry";
	itemType = ITEM_CONSUMABLE;
	type = "cItem";
	description = "Green berry, rich in water";
	this->isOnGround = isOnGround;

	string resPath = getResourcePath() + Item::ITEMS_IMAGES_FOLDER;;
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
		heroEntity->healStatusTimer += HEAL_TIME;
		heroEntity->healStatusAmount = HEAL_AMOUNT / LivingEntity::TIME_TICK;
}
