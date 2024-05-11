#include "door.h"

const int Door::DOOR_STATE_CLOSED = 0;
const int Door::DOOR_STATE_OPENING = 1;
const int Door::DOOR_STATE_OPEN = 2;

const string Door::DOOR_CLOSED_ANIM_SUFFIX = "Closed";
const string Door::DOOR_OPENING_ANIM_SUFFIX = "Opening";
const string Door::DOOR_OPEN_ANIM_SUFFIX = "Open";

Door::Door(AnimationSet* animSet, int id, string prefix, bool isClosed, int posX, int posY, int width, int height, int collisionBoxYOffset) {
	this->animSet = animSet;
	this->id = id;
	animPrefix = prefix;
	this->isClosed = isClosed;
	x = posX;
	y = posY;
	collisionBoxW = width;
	collisionBoxH = height;
	this->collisionBoxYOffset = collisionBoxYOffset;
	type = "door";

	this->isClosed ?
		changeAnimation(DOOR_STATE_CLOSED, true) :
		changeAnimation(DOOR_STATE_OPEN, true);
}

void Door::update() {
	if (!isClosed) {
		collisionBoxW = 0;
		collisionBoxH = 0;
	}
	updateCollisionBox();
	updateCollisions();
	updateAnimation();
}

void Door::changeAnimation(int newState, bool resetFrameToBeginning, string animName) {
	state = newState;

	switch (state) {
		case DOOR_STATE_CLOSED:
			currentAnim = animSet->getAnimation(animPrefix + DOOR_CLOSED_ANIM_SUFFIX);
			break;
		case DOOR_STATE_OPENING:
			currentAnim = animSet->getAnimation(animPrefix + DOOR_OPENING_ANIM_SUFFIX);
			break;
		case DOOR_STATE_OPEN:
			currentAnim = animSet->getAnimation(animPrefix + DOOR_OPEN_ANIM_SUFFIX);
			break;
		default:
			break;
	}

	if (resetFrameToBeginning) {
		currentFrame = currentAnim->getFrame(0);
	}
	else {
		currentFrame = currentAnim->getFrame(currentFrame->frameNumber);
	}
}

void Door::updateAnimation() {
	if (currentFrame == NULL || currentAnim == NULL) {
		return;
	}

	frameTimer += TimeController::timeController.dT;
	//change frames
	if (frameTimer >= currentFrame->duration) {
		//if we are at the end of animation
		if (currentFrame->frameNumber == currentAnim->getEndFrameNumber()) {
			if (state == DOOR_STATE_OPENING) {
				changeAnimation(DOOR_STATE_OPEN, true);
			}
		}
		else {
			// just move to next frame in this animation
			currentFrame = currentAnim->getNextFrame(currentFrame);
		}

		frameTimer = 0;
	}
}

void Door::updateCollisionBox() {
	collisionBox.x = x;
	collisionBox.y = y + collisionBoxYOffset;
	collisionBox.w = collisionBoxW;
	collisionBox.h = collisionBoxH;
}

void Door::open() {
	changeAnimation(DOOR_STATE_OPENING, true);
	isClosed = false;
}
