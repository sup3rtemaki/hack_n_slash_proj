#include "termiteMiner.h"

#include "itemsHub.h"
#include "soundManager.h"

const string TermiteMiner::TERMITE_MINER_ANIM_UP = "up";
const string TermiteMiner::TERMITE_MINER_ANIM_DOWN = "down";
const string TermiteMiner::TERMITE_MINER_ANIM_LEFT = "left";
const string TermiteMiner::TERMITE_MINER_ANIM_RIGHT = "right";
const string TermiteMiner::TERMITE_MINER_ANIM_IDLE_UP = "idleUp";
const string TermiteMiner::TERMITE_MINER_ANIM_IDLE_DOWN = "idleDown";
const string TermiteMiner::TERMITE_MINER_ANIM_IDLE_LEFT = "idleLeft";
const string TermiteMiner::TERMITE_MINER_ANIM_IDLE_RIGHT = "idleRight";
const string TermiteMiner::TERMITE_MINER_ANIM_ATTACK_UP = "attackUp";
const string TermiteMiner::TERMITE_MINER_ANIM_ATTACK_DOWN = "attackDown";
const string TermiteMiner::TERMITE_MINER_ANIM_ATTACK_LEFT = "attackLeft";
const string TermiteMiner::TERMITE_MINER_ANIM_ATTACK_RIGHT = "attackRight";
const string TermiteMiner::TERMITE_MINER_ANIM_DIE_UP = "dieUp";
const string TermiteMiner::TERMITE_MINER_ANIM_DIE_DOWN = "dieDown";
const string TermiteMiner::TERMITE_MINER_ANIM_DIE_LEFT = "dieLeft";
const string TermiteMiner::TERMITE_MINER_ANIM_DIE_RIGHT = "dieRight";

const int TermiteMiner::TERMITE_MINER_STATE_IDLE = 0;
const int TermiteMiner::TERMITE_MINER_STATE_MOVE = 1;
const int TermiteMiner::TERMITE_MINER_STATE_ATTACK = 2;
const int TermiteMiner::TERMITE_MINER_STATE_DEAD = 4;

const int TermiteMiner::TERMITE_MINER_AI_NORMAL = 0;
const int TermiteMiner::TERMITE_MINER_AI_CHASE = 1;

TermiteMiner::TermiteMiner(AnimationSet* animSet) {
	this->animSet = animSet;
	type = "enemy";
	x = Globals::ScreenWidth / 2;
	y = Globals::ScreenHeight / 2;
	essence = 55;
	moveSpeed = 0;
	moveSpeedMax = 20;
	hp = hpMax = 30 + (rand() % 40); //30-49
	damage = 0;
	collisionBoxW = 18;
	collisionBoxH = 20;
	collisionBox.w = collisionBoxW;
	collisionBox.h = collisionBoxH;
	collisionBoxYOffset = -14;
	direction = DIR_DOWN;
	populatePossibleDropItemsMap();
	changeAnimation(TERMITE_MINER_STATE_IDLE, true);
	updateCollisionBox();
}

void TermiteMiner::update() {
	//check if died
	if (hp < 1 && state != TERMITE_MINER_STATE_DEAD) {
		changeAnimation(TERMITE_MINER_STATE_DEAD, true);
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

void TermiteMiner::think() {
	if (state == TERMITE_MINER_STATE_IDLE || state == TERMITE_MINER_STATE_MOVE) {
		thinkTimer -= TimeController::timeController.dT;

		//time to choose an action
		if (thinkTimer <= 0) {
			thinkTimer = rand() % 2; //0 - 2 seconds
			int action = rand() % 10;

			if (action < 2) {
				moving = false;
				aiState = TERMITE_MINER_AI_NORMAL;
				changeAnimation(TERMITE_MINER_STATE_IDLE, true);
			}
			else {
				findNearestTarget();
				if (target != NULL && target->hp > 0) {
					float dist = Entity::distanceBetweenTwoEntities(this, target);

					//if in range, attack
					if (dist < 20) {
						attack();
						aiState = TERMITE_MINER_AI_NORMAL;
					}
					else {
						aiState = TERMITE_MINER_AI_CHASE;
						moving = true;
						changeAnimation(TERMITE_MINER_STATE_MOVE, state != TERMITE_MINER_STATE_MOVE);
					}
				}
				else {
					moving = false;
					aiState = TERMITE_MINER_AI_NORMAL;
					changeAnimation(TERMITE_MINER_STATE_IDLE, true);
				}
			}
		}
	}

	//if chasing a target, then hunt it down
	if (aiState == TERMITE_MINER_AI_CHASE && hp > 0 && active) {
		angle = Entity::angleBetweenTwoEntities(this, target);
		move(angle);
	}
}

void TermiteMiner::attack() {
	moving = false;
	frameTimer = 0;
	changeAnimation(TERMITE_MINER_STATE_ATTACK, true);
}

void TermiteMiner::die() {
	moving = false;
	state = TERMITE_MINER_STATE_DEAD;
	changeAnimation(state, true);

	SoundManager::soundManager.playSound("enemyDie");
}

void TermiteMiner::findNearestTarget() {
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

void TermiteMiner::changeAnimation(int newState, bool resetFrameToBeginning, string animName) {
	state = newState;
	if (state == TERMITE_MINER_STATE_IDLE) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(TERMITE_MINER_ANIM_IDLE_DOWN);
		}
		if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(TERMITE_MINER_ANIM_IDLE_UP);
		}
		if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(TERMITE_MINER_ANIM_IDLE_LEFT);
		}
		if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(TERMITE_MINER_ANIM_IDLE_RIGHT);
		}
	}
	else if (state == TERMITE_MINER_STATE_MOVE) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(TERMITE_MINER_ANIM_DOWN);
		}
		if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(TERMITE_MINER_ANIM_UP);
		}
		if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(TERMITE_MINER_ANIM_LEFT);
		}
		if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(TERMITE_MINER_ANIM_RIGHT);
		}
	}
	else if (state == TERMITE_MINER_STATE_ATTACK) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(TERMITE_MINER_ANIM_ATTACK_DOWN);
		}
		if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(TERMITE_MINER_ANIM_ATTACK_UP);
		}
		if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(TERMITE_MINER_ANIM_ATTACK_LEFT);
		}
		if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(TERMITE_MINER_ANIM_ATTACK_RIGHT);
		}
	}
	else if (state == TERMITE_MINER_STATE_DEAD) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(TERMITE_MINER_ANIM_DIE_DOWN);
		}
		if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(TERMITE_MINER_ANIM_DIE_UP);
		}
		if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(TERMITE_MINER_ANIM_DIE_LEFT);
		}
		if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(TERMITE_MINER_ANIM_DIE_RIGHT);
		}
	}

	if (resetFrameToBeginning) {
		currentFrame = currentAnim->getFrame(0);
	}
	else {
		currentFrame = currentAnim->getFrame(currentFrame->frameNumber);
	}
}

void TermiteMiner::updateAnimation() {
	if (currentFrame == NULL || currentAnim == NULL) {
		return;
	}

	if (state == TERMITE_MINER_STATE_MOVE && !moving) {
		changeAnimation(TERMITE_MINER_STATE_IDLE, true);
	}

	if (state != TERMITE_MINER_STATE_MOVE && moving) {
		changeAnimation(TERMITE_MINER_STATE_MOVE, true);
	}

	frameTimer += TimeController::timeController.dT;

	if (frameTimer >= currentFrame->duration) {

		if (currentFrame->frameNumber == currentAnim->getEndFrameNumber()) {

			if (state == TERMITE_MINER_STATE_ATTACK) {
				changeAnimation(TERMITE_MINER_STATE_MOVE, true);
			}
			else if (state == TERMITE_MINER_STATE_DEAD) {
				frameTimer = 0;
				//if somehow alive, change anim back to moving
				if (hp > 0) {
					changeAnimation(TERMITE_MINER_STATE_MOVE, true);
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

void TermiteMiner::updateDamages() {
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

void TermiteMiner::populatePossibleDropItemsMap() {
}

void TermiteMiner::pursueTarget(LivingEntity* entity) {
	float dist = Entity::distanceBetweenTwoEntities(this, target);
	if ((dist > distanceThreshold)) {
		if (!target->pheromoneTrail.empty()) {
			// TODO fazer com que o inimigo persiga o ponto de feromonio mais proximo dele
			if (currentTargetPos.x == 0 || currentTargetPos.y == 0) {
				currentTargetPos = target->pheromoneTrail.front();
			}
			if (Entity::distanceBetweenTwoPoints(this->x, this->y, currentTargetPos.x, currentTargetPos.y) < (distanceThreshold / 5.f)) {
				currentTargetPos = target->pheromoneTrail.at(pheromoneTrailIndex);
				pheromoneTrailIndex++;
				isChasingPheromone = true;
			}
			else {
				if (isChasingPheromone) {
					currentTargetPos = target->pheromoneTrail.front();
					pheromoneTrailIndex = 1;
				}
				else {
					moving = false;
					aiState = TERMITE_MINER_AI_NORMAL;
					changeAnimation(TERMITE_MINER_STATE_IDLE, true);
					return;
				}
			}
		}
	}
	else {
		currentTargetPos.x = target->x;
		currentTargetPos.y = target->y;
		isChasingPheromone = true;
	}

	//if in range, attack
	if (dist < 50) {
		attack();
		aiState = TERMITE_MINER_AI_NORMAL;
	}
	else {
		aiState = TERMITE_MINER_AI_CHASE;
		moving = true;
		changeAnimation(TERMITE_MINER_STATE_MOVE, state != TERMITE_MINER_STATE_MOVE);
	}
}

