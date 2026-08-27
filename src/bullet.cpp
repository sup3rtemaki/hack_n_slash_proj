#include "bullet.h"
#include "resourceConfig.h"

const string Bullet::BULLET_ANIM_BULLET = "bullet";

Bullet::Bullet(int x, int y) {
	list<DataGroupType> dataGroupTypes;

	localAnimSet = std::make_unique<AnimationSet>();
	localAnimSet->loadAnimationSet(ResourcePaths::ANIMATIONS + "bullet.fdset", dataGroupTypes, Globals::renderer);
	this->animSet = localAnimSet.get();
	solid = false;
	collideWithSolids = true;
	dieOnSolids = true;
	moving = true;
	type = "enemy";
	this->x = x;
	this->y = y;
	moveSpeed = 40;
	moveSpeedMax = 40;
	hp = hpMax = 1;
	damage = 5;
	collisionBoxW = 5;
	collisionBoxH = 5;
	collisionBox.w = collisionBoxW;
	collisionBox.h = collisionBoxH;
	collisionBoxYOffset = 0;
	direction = DIR_DOWN;

	changeAnimation(0, true);
	updateCollisionBox();

	invincibleTimer = 0;
}

void Bullet::update() {
	updateCollisionBox();
	move(angle);
	updateMovement();
	updateCollisions();
	updateHitBox();
	updateDamages();
	updateAnimation();
}

void Bullet::changeAnimation(int newState, bool resetFrameToBeginning, string animName) {
	state = newState;
	currentAnim = animSet->getAnimation(BULLET_ANIM_BULLET);

	if (resetFrameToBeginning) {
		currentFrame = currentAnim->getFrame(0);
	}
	else {
		currentFrame = currentAnim->getFrame(currentFrame->frameNumber);
	}
}

void Bullet::updateAnimation() {
	if (currentFrame == NULL || currentAnim == NULL) {
		return;
	}

	frameTimer += deltaTime;

	if (frameTimer >= currentFrame->duration) {
		if (currentFrame->frameNumber == currentAnim->getEndFrameNumber()) {
			currentFrame = currentAnim->getFrame(0);
		}
		else {
			currentFrame = currentAnim->getNextFrame(currentFrame);
		}

		frameTimer = 0;
	}
}

void Bullet::hitLanded(LivingEntity* entity) {
	//crashed into entity and damaged it, destroy bullet
	active = false;

	//TODO create hit effect
}

void Bullet::crashOntoSolid() {
	//when hit wall, destroy bullet
	active = false;

	//TODO create hit effect
}