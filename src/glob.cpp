#include "glob.h"
#include "itemsHub.h"

const string Glob::GLOB_ANIM_UP = "up";
const string Glob::GLOB_ANIM_DOWN = "down";
const string Glob::GLOB_ANIM_LEFT = "left";
const string Glob::GLOB_ANIM_RIGHT = "right";
const string Glob::GLOB_ANIM_IDLE_UP = "idleUp";
const string Glob::GLOB_ANIM_IDLE_DOWN = "idleDown";
const string Glob::GLOB_ANIM_IDLE_LEFT = "idleLeft";
const string Glob::GLOB_ANIM_IDLE_RIGHT = "idleRight";
const string Glob::GLOB_ANIM_ATTACK_UP = "attackUp";
const string Glob::GLOB_ANIM_ATTACK_DOWN = "attackDown";
const string Glob::GLOB_ANIM_ATTACK_LEFT = "attackLeft";
const string Glob::GLOB_ANIM_ATTACK_RIGHT = "attackRight";
const string Glob::GLOB_ANIM_TELEGRAPH_UP = "telegraphUp";
const string Glob::GLOB_ANIM_TELEGRAPH_DOWN = "telegraphDown";
const string Glob::GLOB_ANIM_TELEGRAPH_LEFT = "telegraphLeft";
const string Glob::GLOB_ANIM_TELEGRAPH_RIGHT = "telegraphRight";
const string Glob::GLOB_ANIM_DIE = "die";

const int Glob::GLOB_STATE_IDLE = 0;
const int Glob::GLOB_STATE_MOVE = 1;
const int Glob::GLOB_STATE_ATTACK = 2;
const int Glob::GLOB_STATE_TELEGRAPH = 3;
const int Glob::GLOB_STATE_DEAD = 4;

const int Glob::GLOB_AI_NORMAL = 0;
const int Glob::GLOB_AI_CHASE = 1;

int Glob::globsKilled = 0;

Glob::Glob(AnimationSet* animSet) {
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
	this->essence = 20;
	populatePossibleDropItemsMap();
	changeAnimation(GLOB_STATE_IDLE, true);
	updateCollisionBox();
}

void Glob::update() {
	//check if died
	if (hp < 1 && state != GLOB_STATE_DEAD) {
		changeAnimation(GLOB_STATE_DEAD, true);
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

void Glob::think() {
	if (state == GLOB_STATE_IDLE || state == GLOB_STATE_MOVE) {
		thinkTimer -= TimeController::timeController.dT;

		//time to choose an action
		if (thinkTimer <= 0) {
			thinkTimer = rand() % 5; //0 - 5 seconds
			int action = rand() % 10;

			if (action < 3) {
				moving = false;
				aiState = GLOB_AI_NORMAL;
				changeAnimation(GLOB_STATE_IDLE, true);
			}
			else {
				findNearestTarget();
				if (target != nullptr && target->hp > 0) {
					float dist = Entity::distanceBetweenTwoEntities(this, target);
					
					//if in range, attack
					if (dist < 100) {
						telegraph();
						aiState = GLOB_AI_NORMAL;
					}
					else {
						aiState = GLOB_AI_CHASE;
						moving = true;
						changeAnimation(GLOB_STATE_MOVE, state != GLOB_STATE_MOVE);
					}
				}
				else {
					moving = false;
					aiState = GLOB_AI_NORMAL;
					changeAnimation(GLOB_STATE_IDLE, true);
				}
			}
		}
	}

	//if chasing a target, then hunt it down
	if (aiState == GLOB_AI_CHASE && hp > 0 && active) {
		angle = Entity::angleBetweenTwoEntities(this, target);
		move(angle);
	}
}

void Glob::telegraph() {
	moving = false;
	frameTimer = 0;
	changeAnimation(GLOB_STATE_TELEGRAPH, true);
}

void Glob::attack() {
	moving = false;
	frameTimer = 0;
	slideAmount = 100;
	slideAngle = angle;
	changeAnimation(GLOB_STATE_ATTACK, true);
}

void Glob::die() {
	moving = false;
	state = GLOB_STATE_DEAD;
	changeAnimation(state, true);

	SoundManager::soundManager.playSound("enemyDie");

	//add to score
	Glob::globsKilled++;
}

void Glob::findNearestTarget() {
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

void Glob::changeAnimation(int newState, bool resetFrameToBeginning, string animName) {
	state = newState;
	if (state == GLOB_STATE_IDLE) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(GLOB_ANIM_IDLE_DOWN);
		}
		if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(GLOB_ANIM_IDLE_UP);
		}
		if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(GLOB_ANIM_IDLE_LEFT);
		}
		if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(GLOB_ANIM_IDLE_RIGHT);
		}
	}
	else if (state == GLOB_STATE_MOVE) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(GLOB_ANIM_DOWN);
		}
		if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(GLOB_ANIM_UP);
		}
		if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(GLOB_ANIM_LEFT);
		}
		if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(GLOB_ANIM_RIGHT);
		}
	}
	else if (state == GLOB_STATE_ATTACK) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(GLOB_ANIM_ATTACK_DOWN);
		}
		if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(GLOB_ANIM_ATTACK_UP);
		}
		if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(GLOB_ANIM_ATTACK_LEFT);
		}
		if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(GLOB_ANIM_ATTACK_RIGHT);
		}
	}
	else if (state == GLOB_STATE_TELEGRAPH) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(GLOB_ANIM_TELEGRAPH_DOWN);
		}
		if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(GLOB_ANIM_TELEGRAPH_UP);
		}
		if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(GLOB_ANIM_TELEGRAPH_LEFT);
		}
		if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(GLOB_ANIM_TELEGRAPH_RIGHT);
		}
	}
	else if (state == GLOB_STATE_DEAD) {
		currentAnim = animSet->getAnimation(GLOB_ANIM_DIE);
	}

	if (resetFrameToBeginning) {
		currentFrame = currentAnim->getFrame(0);
	}
	else {
		currentFrame = currentAnim->getFrame(currentFrame->frameNumber);
	}
}

void Glob::updateAnimation() {
	if (currentFrame == NULL || currentAnim == NULL) {
		return;
	}

	if (state == GLOB_STATE_MOVE && !moving) {
		changeAnimation(GLOB_STATE_IDLE, true);
	}

	if (state != GLOB_STATE_MOVE && moving) {
		changeAnimation(GLOB_STATE_MOVE, true);
	}

	frameTimer += TimeController::timeController.dT;

	if (frameTimer >= currentFrame->duration) {

		if (currentFrame->frameNumber == currentAnim->getEndFrameNumber()) {

			if (state == GLOB_STATE_TELEGRAPH) {
				attack();
			}
			else if (state == GLOB_STATE_ATTACK) {
				changeAnimation(GLOB_STATE_MOVE, true);
			}
			else if (state == GLOB_STATE_DEAD) {
				frameTimer = 0;
				//if somehow alive, change anim back to moving
				if (hp > 0) {
					changeAnimation(GLOB_STATE_MOVE, true);
				}
				else {
					active = false;

					checkIfDropsItem();
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

void Glob::updateDamages() {
	if (active && hp > 0 && invincibleTimer <= 0) {
		for (auto entity = Entity::entities.begin(); entity != Entity::entities.end(); entity++) {
			if ((*entity)->active && ((*entity)->type == "hero" || (*entity)->type == "projectile")) {
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

void Glob::populatePossibleDropItemsMap(){
	//TODO: Popular de verdade
	possibleDropItemsMap.insert({ Item::HONEYDEW_POTION_ID, {{0, 15}, 3} });
	possibleDropItemsMap.insert({ Item::GREEN_BERRY_ID, {{0, 50}, 3} });
}
