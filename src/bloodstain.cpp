#include "bloodstain.h"

const string Bloodstain::BLOODSTAIN_ANIM_IDLE = "idle";
const string Bloodstain::BLOODSTAIN_ANIM_DIE = "die";

const int Bloodstain::BLOODSTAIN_STATE_IDLE = 0;
const int Bloodstain::BLOODSTAIN_STATE_DIE = 1;

Bloodstain::Bloodstain() {
	list<DataGroupType> dataGroupTypes;
	DataGroupType colBoxType;
	colBoxType.groupName = "collisionBox";
	colBoxType.dataType = DataGroupType::DATATYPE_BOX;

	DataGroupType hitBoxType;
	hitBoxType.groupName = "hitBox";
	hitBoxType.dataType = DataGroupType::DATATYPE_BOX;

	DataGroupType dmgType;
	dmgType.groupName = "damage";
	dmgType.dataType = DataGroupType::DATATYPE_NUMBER;

	dataGroupTypes.push_back(colBoxType);
	dataGroupTypes.push_back(hitBoxType);
	dataGroupTypes.push_back(dmgType);

	localAnimSet = std::make_unique<AnimationSet>();
	localAnimSet->loadAnimationSet("Assets\\Animations\\bloodstain.fdset", dataGroupTypes);
	this->animSet = localAnimSet.get();

	type = "bloodstain";
	changeAnimation(BLOODSTAIN_STATE_IDLE, true);
}

Bloodstain::~Bloodstain() {
}

void Bloodstain::update() {
	updateAnimation();
}

void Bloodstain::changeAnimation(int newState, bool resetFrameToBeginning, string animName) {
	state = newState;

	if (state == BLOODSTAIN_STATE_IDLE) {
		currentAnim = animSet->getAnimation(BLOODSTAIN_ANIM_IDLE);
	}
	else if (state == BLOODSTAIN_STATE_DIE) {
		currentAnim = animSet->getAnimation(BLOODSTAIN_ANIM_DIE);
	}

	if (resetFrameToBeginning) {
		currentFrame = currentAnim->getFrame(0);
	}
	else {
		currentFrame = currentAnim->getFrame(currentFrame->frameNumber);
	}
}

void Bloodstain::updateAnimation() {
	if (currentFrame == NULL || currentAnim == NULL) {
		return;
	}

	frameTimer += deltaTime;

	if (frameTimer >= currentFrame->duration) {
		if (currentFrame->frameNumber == currentAnim->getEndFrameNumber()) { // end of animation
			if (state == BLOODSTAIN_STATE_DIE) {
				currentAnim = NULL;
				x = -1;
				y = -1;
			}
			else {
				currentFrame = currentAnim->getFrame(0);
			}
		}
		else {
			currentFrame = currentAnim->getNextFrame(currentFrame);
		}

		frameTimer = 0;
	}
}

void Bloodstain::setLocation(int posX, int posY, int essence, string mapName) {
	this->x = prevX = posX;
	this->y = prevY = posY;
	this->essence = essence;
	this->mapName = mapName;
	isLive = true;
}

void Bloodstain::create() {
	x = prevX;
	y = prevY;
	changeAnimation(BLOODSTAIN_STATE_IDLE, true);
}

void Bloodstain::destroy() {
	changeAnimation(BLOODSTAIN_STATE_DIE, true);
}

int Bloodstain::recoverEssence() {
	destroy();
	isLive = false;
	int essenceAux = this->essence;
	this->essence = 0;
	return essenceAux;;
}
