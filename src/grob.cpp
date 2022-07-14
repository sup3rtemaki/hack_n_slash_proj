#include "grob.h"

const string Grob::GROB_ANIM_UP = "up";
const string Grob::GROB_ANIM_DOWN = "down";
const string Grob::GROB_ANIM_LEFT = "left";
const string Grob::GROB_ANIM_RIGHT = "right";
const string Grob::GROB_ANIM_IDLE_UP = "idleUp";
const string Grob::GROB_ANIM_IDLE_DOWN = "idleDown";
const string Grob::GROB_ANIM_IDLE_LEFT = "idleLeft";
const string Grob::GROB_ANIM_IDLE_RIGHT = "idleRight";
const string Grob::GROB_ANIM_ATTACK_UP = "attackUp";
const string Grob::GROB_ANIM_ATTACK_DOWN = "attackDown";
const string Grob::GROB_ANIM_ATTACK_LEFT = "attackLeft";
const string Grob::GROB_ANIM_ATTACK_RIGHT = "attackRight";
const string Grob::GROB_ANIM_TELEGRAPH_UP = "telegraphUp";
const string Grob::GROB_ANIM_TELEGRAPH_DOWN = "telegraphDown";
const string Grob::GROB_ANIM_TELEGRAPH_LEFT = "telegraphLeft";
const string Grob::GROB_ANIM_TELEGRAPH_RIGHT = "telegraphRight";
const string Grob::GROB_ANIM_DIE = "die";

const int Grob::GROB_STATE_IDLE = 0;
const int Grob::GROB_STATE_MOVE = 1;
const int Grob::GROB_STATE_ATTACK = 2;
const int Grob::GROB_STATE_TELEGRAPH = 3;
const int Grob::GROB_STATE_DEAD = 4;

const int Grob::GROB_AI_NORMAL = 0;
const int Grob::GROB_AI_CHASE = 1;

int Grob::grobsKilled = 0;

Grob::Grob(AnimationSet* animSet) {
	this->animSet = animSet;
	type = "enemy";
	x = Globals::ScreenWidth / 2;
	y = Globals::ScreenHeight / 2;
	moveSpeed = 0;
	moveSpeedMax = 20;
	hp = hpMax = 10 + (rand() % 20); //10-29
	damage = 0;
	collisionBoxW = 18;
	collisionBoxH = 20;
	collisionBox.w = collisionBoxW;
	collisionBox.h = collisionBoxH;
	collisionBoxYOffset = -14;
	direction = DIR_DOWN;
	changeAnimation(GROB_STATE_IDLE, true);
	updateCollisionBox();
}

void Grob::update() {
	//check if died
	if (hp < 1 && state != GROB_STATE_DEAD) {
		changeAnimation(GROB_STATE_DEAD, true);
		moving = false;
		die();
	}

	think();

	updateCollisionBox();
	updateMovement();
	updateCollisions();
	updateHitBox();
	updateDamages();
	updateAnimation();
	updateInvincibleTimer();
}

void Grob::think() {
	if (state == GROB_STATE_IDLE || state == GROB_STATE_MOVE) {
		thinkTimer -= TimeController::timeController.dT;

		//time to choose an action
		if (thinkTimer <= 0) {
			thinkTimer = rand() % 5; //0 - 5 seconds
			int action = rand() % 10;

			if (action < 3) {
				moving = false;
				aiState = GROB_AI_NORMAL;
				changeAnimation(GROB_STATE_IDLE, true);
			}
			else {
				findNearestTarget();
				if (target != NULL && target->hp > 0) {
					//float dist = Entity::distanceBetweenTwoEntities(this, target);

					//if in range, attack
					//if (dist < 100) {
					//	telegraph();
					//	aiState = GROB_AI_NORMAL;
					//}
					//else {
					aiState = GROB_AI_CHASE;
					moving = true;
					changeAnimation(GROB_STATE_MOVE, state != GROB_STATE_MOVE);
					//}
				}
				else {
					moving = false;
					aiState = GROB_AI_NORMAL;
					changeAnimation(GROB_STATE_IDLE, true);
				}
			}
		}
	}

	//if chasing a target, then hunt it down
	if (aiState == GROB_AI_CHASE && hp > 0 && active) {
		angle = Entity::angleBetweenTwoEntities(this, target);
		move(angle);
	}
}

void Grob::telegraph() {
	moving = false;
	frameTimer = 0;
	changeAnimation(GROB_STATE_TELEGRAPH, true);
}

void Grob::attack() {
	moving = false;
	frameTimer = 0;
	slideAmount = 100;
	slideAngle = angle;
	changeAnimation(GROB_STATE_ATTACK, true);
}

void Grob::die() {
	moving = false;
	state = GROB_STATE_DEAD;
	changeAnimation(state, true);

	SoundManager::soundManager.playSound("enemyDie");

	//add to score
	Grob::grobsKilled++;
}

void Grob::findNearestTarget() {
	target = NULL;

	//find closest target
	for (auto entity = Entity::entities.begin(); entity != Entity::entities.end(); entity++) {
		if ((*entity)->type == "hero" && (*entity)->active) {
			//found first hero in list, set it as target
			if (target == NULL) {
				target = (LivingEntity*)(*entity);
			}
			else {
				//is this other hero closer than previous target
				if (Entity::distanceBetweenTwoEntities(this, (*entity)) < Entity::distanceBetweenTwoEntities(this, target)) {
					target = (LivingEntity*)(*entity);
				}
			}
		}
	}
}

void Grob::changeAnimation(int newState, bool resetFrameToBeginning, string animName) {
	state = newState;
	if (state == GROB_STATE_IDLE) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(GROB_ANIM_IDLE_DOWN);
		}
		if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(GROB_ANIM_IDLE_UP);
		}
		if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(GROB_ANIM_IDLE_LEFT);
		}
		if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(GROB_ANIM_IDLE_RIGHT);
		}
	}
	else if (state == GROB_STATE_MOVE) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(GROB_ANIM_DOWN);
		}
		if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(GROB_ANIM_UP);
		}
		if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(GROB_ANIM_LEFT);
		}
		if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(GROB_ANIM_RIGHT);
		}
	}
	else if (state == GROB_STATE_ATTACK) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(GROB_ANIM_ATTACK_DOWN);
		}
		if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(GROB_ANIM_ATTACK_UP);
		}
		if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(GROB_ANIM_ATTACK_LEFT);
		}
		if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(GROB_ANIM_ATTACK_RIGHT);
		}
	}
	else if (state == GROB_STATE_TELEGRAPH) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(GROB_ANIM_TELEGRAPH_DOWN);
		}
		if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(GROB_ANIM_TELEGRAPH_UP);
		}
		if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(GROB_ANIM_TELEGRAPH_LEFT);
		}
		if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(GROB_ANIM_TELEGRAPH_RIGHT);
		}
	}
	else if (state == GROB_STATE_DEAD) {
		currentAnim = animSet->getAnimation(GROB_ANIM_DIE);
	}

	if (resetFrameToBeginning) {
		currentFrame = currentAnim->getFrame(0);
	}
	else {
		currentFrame = currentAnim->getFrame(currentFrame->frameNumber);
	}
}

void Grob::updateAnimation() {
	if (currentFrame == NULL || currentAnim == NULL) {
		return;
	}

	if (state == GROB_STATE_MOVE && !moving) {
		changeAnimation(GROB_STATE_IDLE, true);
	}

	if (state != GROB_STATE_MOVE && moving) {
		changeAnimation(GROB_STATE_MOVE, true);
	}

	frameTimer += TimeController::timeController.dT;

	if (frameTimer >= currentFrame->duration) {

		if (currentFrame->frameNumber == currentAnim->getEndFrameNumber()) {

			if (state == GROB_STATE_TELEGRAPH) {
				attack();
			}
			else if (state == GROB_STATE_ATTACK) {
				changeAnimation(GROB_STATE_MOVE, true);
			}
			else if (state == GROB_STATE_DEAD) {
				frameTimer = 0;
				//if somehow alive, change anim back to moving
				if (hp > 0) {
					changeAnimation(GROB_STATE_MOVE, true);
				}
				else {
					active = false;
				}
			}
			else {
				//loop animation
				currentFrame = currentAnim->getFrame(0);
			}
		}
		else {
			// just move to next frame
			currentFrame = currentAnim->getNextFrame(currentFrame);
		}

		frameTimer = 0;
	}
}

void Grob::updateDamages() {
	if (active && hp > 0 && invincibleTimer <= 0) {
		for (auto entity = Entity::entities.begin(); entity != Entity::entities.end(); entity++) {
			if ((*entity)->active && (*entity)->type == "hero") {
				//refrence as living entity, so we can access hitbox/damage
				LivingEntity* enemy = (LivingEntity*)(*entity);

				//if enemy hits me
				if (enemy->damage > 0 && Entity::checkCollision(collisionBox, enemy->hitBox)) {
					enemy->hitLanded(this); // let attacker know they hit
					hp -= enemy->damage;

					if (hp > 0) {
						SoundManager::soundManager.playSound("enemyHit");
						invincibleTimer = 0.1;
					}
					//get thrown backwards
					slideAngle = Entity::angleBetweenTwoEntities((*entity), this);
					slideAmount = 300;
				}
			}
		}
	}
}
