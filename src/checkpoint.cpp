#include "checkpoint.h"
#include "resourceConfig.h"

const string Checkpoint::CHECKPOINT_ANIM_ACTIVE = "active";
const string Checkpoint::CHECKPOINT_ANIM_INACTIVE = "inactive";

const int Checkpoint::CHECKPOINT_STATE_ACTIVE = 0;
const int Checkpoint::CHECKPOINT_STATE_INACTIVE = 1;

Checkpoint::Checkpoint(int id, const string& mapFileName) {
	list<DataGroupType> dataGroupTypes;
	DataGroupType colBoxType; colBoxType.groupName = "collisionBox"; colBoxType.dataType = DataGroupType::DATATYPE_BOX;
	DataGroupType hitBoxType; hitBoxType.groupName = "hitBox"; hitBoxType.dataType = DataGroupType::DATATYPE_BOX;
	DataGroupType dmgType; dmgType.groupName = "damage"; dmgType.dataType = DataGroupType::DATATYPE_NUMBER;
	dataGroupTypes.push_back(colBoxType);
	dataGroupTypes.push_back(hitBoxType);
	dataGroupTypes.push_back(dmgType);

	localAnimSet = std::make_unique<AnimationSet>();
	localAnimSet->loadAnimationSet(ResourcePaths::ANIMATIONS + "checkpoint.fdset", dataGroupTypes);
	this->animSet = localAnimSet.get();
	this->id = id;
	this->mapFileName = mapFileName;
	this->solid = true;
	this->type = "checkpoint";
	collisionBoxW = 10;
	collisionBoxH = 10;
	collisionBoxYOffset = -10;
	isActivated = false;

	changeAnimation(CHECKPOINT_STATE_INACTIVE, true);
	updateCollisionBox();
}

void Checkpoint::update() {
	updateCollisionBox();
	updateCollisions();
	updateAnimation();
}

void Checkpoint::updateAnimation() {
	if (currentFrame == NULL || currentAnim == NULL) {
		return;
	}

	frameTimer += deltaTime;
	//change frames
	if (frameTimer >= currentFrame->duration) {
		//if we are at the end of animation
		if (currentFrame->frameNumber == currentAnim->getEndFrameNumber()) {
			currentFrame = currentAnim->getFrame(0);
		}
		else {
			currentFrame = currentAnim->getNextFrame(currentFrame);
		}

		frameTimer = 0;
	}
}

void Checkpoint::activate() {
	frameTimer = 0;
	isActivated = true;
	changeAnimation(CHECKPOINT_STATE_ACTIVE, true);
}

void Checkpoint::changeAnimation(int newState, bool resetFrameToBeginning, string animName) {
	state = newState;

	if (state == CHECKPOINT_STATE_ACTIVE) {
		currentAnim = animSet->getAnimation(CHECKPOINT_ANIM_ACTIVE);
	}
	else if (state == CHECKPOINT_STATE_INACTIVE) {
		currentAnim = animSet->getAnimation(CHECKPOINT_ANIM_INACTIVE);
	}

	if (resetFrameToBeginning) {
		currentFrame = currentAnim->getFrame(0);
	}
	else {
		currentFrame = currentAnim->getFrame(currentFrame->frameNumber);
	}
}
