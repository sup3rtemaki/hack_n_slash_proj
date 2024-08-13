#include "checkpoint.h"

const string Checkpoint::CHECKPOINT_ANIM_ACTIVE = "active";
const string Checkpoint::CHECKPOINT_ANIM_INACTIVE = "inactive";

const int Checkpoint::CHECKPOINT_STATE_ACTIVE = 0;
const int Checkpoint::CHECKPOINT_STATE_INACTIVE = 1;

Checkpoint::Checkpoint(AnimationSet* animSet, int id, string& mapFileName) {
	this->animSet = animSet;
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

	frameTimer += TimeController::timeController.dT;
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
