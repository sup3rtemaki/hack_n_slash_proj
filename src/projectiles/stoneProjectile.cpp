#include "projectiles/stoneProjectile.h"

const string StoneProjectile::STONE_PROJECTILE_ANIM_THROW_UP_LEFT = "throwUpLeft";
const string StoneProjectile::STONE_PROJECTILE_ANIM_THROW_DOWN_RIGHT = "throwDownRight";
const string StoneProjectile::STONE_PROJECTILE_ANIM_DESTROY = "destroy";

 const int StoneProjectile::STONE_PROJECTILE_STATE_UP_LEFT = 0;
 const int StoneProjectile::STONE_PROJECTILE_STATE_DOWN_RIGHT = 1;
 const int StoneProjectile::STONE_PROJECTILE_STATE_DESTROY = 2;

StoneProjectile::StoneProjectile(AnimationSet* animSet, int x, int y) {
	this->animSet = animSet;
	solid = false;
	collideWithSolids = true;
	dieOnSolids = true;
	moving = true;
	type = "projectile";
	this->x = x;
	this->y = y;
	moveSpeed = 70.f;
	moveSpeedMax = 70.f;
	hp = hpMax = 1.f;
	damage = 10;
	collisionBoxW = 5;
	collisionBoxH = 5;
	collisionBox.w = collisionBoxW;
	collisionBox.h = collisionBoxH;
	collisionBoxYOffset = 0.f;
	direction = DIR_DOWN;
	lifetime = 1.f;
	lifetimeTimer = 0.f;

	changeAnimation(0, true);
	updateCollisionBox();

	invincibleTimer = 0.f;
}

void StoneProjectile::update() {
	updateCollisionBox();

	if ((hp < 1 && state != STONE_PROJECTILE_STATE_DESTROY) || 
		(lifetimeTimer > lifetime)) {
		// Descomentar linha abaixo caso queira que o item pare de se mover ao 
		// ser destruído pela distancia
		// hp = 0;
		lifetimeTimer = 0;
		moving = false;
		die();
	}
	else if (hp >= 1) {
		lifetimeTimer += TimeController::timeController.dT;
		move(angle);
		updateMovement();
	}

	updateCollisions();
	updateHitBox();
	updateDamages();
	updateAnimation();
}

void StoneProjectile::die() {
	changeAnimation(STONE_PROJECTILE_STATE_DESTROY, true);
}

void StoneProjectile::changeAnimation(int newState, bool resetFrameToBeginning, string animName) {
	state = newState;

	if (state == STONE_PROJECTILE_STATE_UP_LEFT) {
		currentAnim = animSet->getAnimation(STONE_PROJECTILE_ANIM_THROW_UP_LEFT);
	}
	else if (state == STONE_PROJECTILE_STATE_DOWN_RIGHT) {
		currentAnim = animSet->getAnimation(STONE_PROJECTILE_ANIM_THROW_DOWN_RIGHT);
	}
	else if (state == STONE_PROJECTILE_STATE_DESTROY) {
		currentAnim = animSet->getAnimation(STONE_PROJECTILE_ANIM_DESTROY);
	}

	if (resetFrameToBeginning) {
		currentFrame = currentAnim->getFrame(0);
	}
	else {
		currentFrame = currentAnim->getFrame(currentFrame->frameNumber);
	}
}

void StoneProjectile::updateAnimation() {
	if (currentFrame == NULL || currentAnim == NULL) {
		return;
	}

	frameTimer += TimeController::timeController.dT;

	if (frameTimer >= currentFrame->duration) {
		if (currentFrame->frameNumber == currentAnim->getEndFrameNumber()) {
			if (state == STONE_PROJECTILE_STATE_DESTROY) {
				active = false;
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

void StoneProjectile::hitLanded(LivingEntity* entity) {
	//crashed into entity and damaged it, destroy bullet
	hp = 0;
	
	//TODO create hit effect
}

void StoneProjectile::crashOntoSolid() {
	//when hit wall, destroy bullet
	hp = 0;

	//TODO create hit effect
}
