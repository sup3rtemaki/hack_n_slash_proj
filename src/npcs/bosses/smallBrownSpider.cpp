#include "npcs\bosses\smallBrownSpider.h"

#include "soundManager.h"

// animations
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_UP = "up";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_DOWN = "down";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_LEFT = "left";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_RIGHT = "right";

const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_IDLE_UP = "idleUp";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_IDLE_DOWN = "idleDown";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_IDLE_LEFT = "idleLeft";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_IDLE_RIGHT = "idleRight";

const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_TELEGRAPH_DASH_UP = "telegraphDashUp";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_TELEGRAPH_DASH_DOWN = "telegraphDashDown";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_TELEGRAPH_DASH_LEFT = "telegraphDashLeft";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_TELEGRAPH_DASH_RIGHT = "telegraphDashRight";

const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_DASH_UP = "dashUp";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_DASH_DOWN = "dashDown";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_DASH_LEFT = "dashLeft";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_DASH_RIGHT = "dashRight";

const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_SLASH_RHAND_UP = "slashRHandUp";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_SLASH_LHAND_UP = "slashLHandUp";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_SLASH_RHAND_DOWN = "slashRHandDown";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_SLASH_LHAND_DOWN = "slashLHandDown";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_SLASH_RHAND_LEFT = "slashRHandLeft";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_SLASH_LHAND_LEFT = "slashLHandLeft";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_SLASH_RHAND_RIGHT = "slashRHandRight";
const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_SLASH_LHAND_RIGHT = "slashLHandRight";

const string SmallBrownSpider::SMALL_BROWN_SPIDER_ANIM_DIE = "die";

// states
const int SmallBrownSpider::SMALL_BROWN_SPIDER_STATE_IDLE = 0;
const int SmallBrownSpider::SMALL_BROWN_SPIDER_STATE_MOVE = 1;
const int SmallBrownSpider::SMALL_BROWN_SPIDER_STATE_SLASH = 2;
const int SmallBrownSpider::SMALL_BROWN_SPIDER_STATE_DASH = 3;
const int SmallBrownSpider::SMALL_BROWN_SPIDER_STATE_TELEGRAPH_DASH = 4;
const int SmallBrownSpider::SMALL_BROWN_SPIDER_STATE_DEAD = 5;

//phases
const int SmallBrownSpider::SMALL_BROWN_SPIDER_PHASE_NORMAL = 1;
const int SmallBrownSpider::SMALL_BROWN_SPIDER_PHASE_DAMAGED = 2;
const int SmallBrownSpider::SMALL_BROWN_SPIDER_PHASE_FRANTIC = 3;

// ai
const int SmallBrownSpider::SMALL_BROWN_SPIDER_AI_NORMAL = 0;
const int SmallBrownSpider::SMALL_BROWN_SPIDER_AI_CHASE = 1;

SmallBrownSpider::SmallBrownSpider(AnimationSet* animSet) {
	this->animSet = animSet;
	type = "boss";
	id = 1;
	x = Globals::ScreenWidth - 32;
	y = Globals::ScreenHeight - 32;
	moveSpeed = 0;
	moveSpeedMax = 40;
	hp = hpMax = 500;
	damage = 0;
	collisionBoxW = 40;
	collisionBoxH = 40;
	collisionBox.w = collisionBoxW;
	collisionBox.h = collisionBoxH;
	collisionBoxYOffset = -40;

	changeAnimation(SMALL_BROWN_SPIDER_STATE_IDLE, true);
	updateCollisionBox();
}

void SmallBrownSpider::update() {
	//check if dead
	if (hp < 1 && state != SMALL_BROWN_SPIDER_STATE_DEAD) {
		changeAnimation(SMALL_BROWN_SPIDER_STATE_DEAD, true);
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

void SmallBrownSpider::think() {
	if (state == SMALL_BROWN_SPIDER_STATE_IDLE || state == SMALL_BROWN_SPIDER_STATE_MOVE) {
		thinkTimer -= TimeController::timeController.dT;

		//time to choose an action
		if (thinkTimer <= 0) {
			thinkTimer = rand() % 2; //0 - 5 seconds
			int action = rand() % 10;

			if (action < 3) {
				moving = false;
				aiState = SMALL_BROWN_SPIDER_AI_NORMAL;
				changeAnimation(SMALL_BROWN_SPIDER_STATE_IDLE, true);
			}
			else {
				findNearestTarget();
				if (target != nullptr && target->hp > 0) {
					float dist = Entity::distanceBetweenTwoEntities(this, target);

					//if in range, attack
					if (dist < 50) {
						action <= 4.f ? telegraphDash() : slash();
						aiState = SMALL_BROWN_SPIDER_AI_NORMAL;
					}
					else {
						aiState = SMALL_BROWN_SPIDER_AI_CHASE;
						moving = true;
						changeAnimation(SMALL_BROWN_SPIDER_STATE_MOVE, state != SMALL_BROWN_SPIDER_STATE_MOVE);
					}
				}
				else {
					moving = false;
					aiState = SMALL_BROWN_SPIDER_AI_NORMAL;
					changeAnimation(SMALL_BROWN_SPIDER_STATE_IDLE, true);
				}
			}
		}
	}

	//if chasing a target, then hunt it down
	if (aiState == SMALL_BROWN_SPIDER_AI_CHASE && hp > 0 && active) {
		angle = Entity::angleBetweenTwoEntities(this, target);
		move(angle);
	}
}

void SmallBrownSpider::slash() {
	moving = false;
	changeAnimation(SMALL_BROWN_SPIDER_STATE_SLASH, true);
}

void SmallBrownSpider::dash() {
	moving = false;
	frameTimer = 0;
	slideAmount = 300;
	slideAngle = angle;
	changeAnimation(SMALL_BROWN_SPIDER_STATE_DASH, true);
}

void SmallBrownSpider::telegraphDash() {
	moving = false;
	frameTimer = 0;
	changeAnimation(SMALL_BROWN_SPIDER_STATE_TELEGRAPH_DASH, true);
}

void SmallBrownSpider::die() {
	moving = false;
	changeAnimation(SMALL_BROWN_SPIDER_STATE_DEAD, true);
}

void SmallBrownSpider::findNearestTarget() {
	//TODO cleaner solution (make another class to help targeting, make glob, grob and king inherit from it)

	target = nullptr;

	//find closest target
	for (auto entity = Entity::entities.begin(); entity != Entity::entities.end(); entity++) {
		if ((*entity)->type == "hero" && (*entity)->active) {
			//found first hero in list, set it as target
			if (target == nullptr) {
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

void SmallBrownSpider::changeAnimation(int newState, bool resetFrameToBeginning, string animName) {
	state = newState;

	if (state == SMALL_BROWN_SPIDER_STATE_IDLE) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_IDLE_DOWN);
		}
		else if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_IDLE_UP);
		}
		else if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_IDLE_LEFT);
		}
		else if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_IDLE_RIGHT);
		}
	}
	else if (state == SMALL_BROWN_SPIDER_STATE_MOVE) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_DOWN);
		}
		else if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_UP);
		}
		else if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_LEFT);
		}
		else if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_RIGHT);
		}
	}
	else if (state == SMALL_BROWN_SPIDER_STATE_SLASH) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_SLASH_RHAND_DOWN);
		}
		else if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_SLASH_RHAND_UP);
		}
		else if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_SLASH_RHAND_LEFT);
		}
		else if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_SLASH_RHAND_RIGHT);
		}
	}
	else if (state == SMALL_BROWN_SPIDER_STATE_DASH) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_DASH_DOWN);
		}
		else if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_DASH_UP);
		}
		else if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_DASH_LEFT);
		}
		else if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_DASH_RIGHT);
		}
	}
	else if (state == SMALL_BROWN_SPIDER_STATE_TELEGRAPH_DASH) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_TELEGRAPH_DASH_DOWN);
		}
		else if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_TELEGRAPH_DASH_UP);
		}
		else if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_TELEGRAPH_DASH_LEFT);
		}
		else if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_TELEGRAPH_DASH_RIGHT);
		}
	}
	else if (state == SMALL_BROWN_SPIDER_STATE_DEAD) {
		currentAnim = animSet->getAnimation(SMALL_BROWN_SPIDER_ANIM_DIE);
	}

	if (resetFrameToBeginning) {
		currentFrame = currentAnim->getFrame(0);
	}
	else {
		currentFrame = currentAnim->getFrame(currentFrame->frameNumber);
	}
}

void SmallBrownSpider::updateAnimation() {
	if (currentFrame == NULL || currentAnim == NULL) {
		return;
	}

	if (state == SMALL_BROWN_SPIDER_STATE_MOVE && !moving) {
		changeAnimation(SMALL_BROWN_SPIDER_STATE_IDLE, true);
	}

	if (state != SMALL_BROWN_SPIDER_STATE_MOVE && moving) {
		changeAnimation(SMALL_BROWN_SPIDER_STATE_MOVE, true);
	}

	frameTimer += TimeController::timeController.dT;
	//change frames
	if (frameTimer >= currentFrame->duration) {
		//if we are at the end of animation
		if (currentFrame->frameNumber == currentAnim->getEndFrameNumber()) {
			if (state == SMALL_BROWN_SPIDER_STATE_TELEGRAPH_DASH) {
				dash();
			}
			else if (state == SMALL_BROWN_SPIDER_STATE_SLASH || state == SMALL_BROWN_SPIDER_STATE_DASH) {
				//change back to moving state/anim
				changeAnimation(SMALL_BROWN_SPIDER_STATE_IDLE, true);
			}
			else if (state == SMALL_BROWN_SPIDER_STATE_DEAD) {
				frameTimer = 0;
				//if somehow alive, change anim back to moving
				if (hp > 0) {
					changeAnimation(SMALL_BROWN_SPIDER_STATE_MOVE, true);
				}
				else {
					active = false;
					checkIfDropsItem();
				}
			}
			else {
				// reset animation (loops it)
				currentFrame = currentAnim->getFrame(0);
			}
		}
		else {
			// just move to next frame in this animation
			currentFrame = currentAnim->getNextFrame(currentFrame);
		}

		frameTimer = 0;
	}
}

void SmallBrownSpider::updateDamages() {
	//am i hittable
	if (active && hp > 0 && invincibleTimer <= 0) {
		for (auto entity = Entity::entities.begin(); entity != Entity::entities.end(); entity++) {
			if ((*entity)->active && (*entity)->type == "hero") {
				//cast entity pointer to livingentity pointer
				LivingEntity* enemy = (LivingEntity*)(*entity);

				//if enemy is hitting me
				if (enemy->damage > 0 && Entity::checkCollision(collisionBox, enemy->hitBox)) {
					enemy->hitLanded(this);
					hp -= enemy->damage;

					if (hp > 0) {
						SoundManager::soundManager.playSound("enemyHit");
						invincibleTimer = 0.1;
					}
				}
			}
		}
	}
}
