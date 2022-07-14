#include "hero.h"
#include "honeydewPotion.h"

const string Hero::HERO_ANIM_UP = "up";
const string Hero::HERO_ANIM_DOWN = "down";
const string Hero::HERO_ANIM_LEFT = "left";
const string Hero::HERO_ANIM_RIGHT = "right";

const string Hero::HERO_ANIM_IDLE_UP = "idleUp";
const string Hero::HERO_ANIM_IDLE_DOWN = "idleDown";
const string Hero::HERO_ANIM_IDLE_LEFT = "idleLeft";
const string Hero::HERO_ANIM_IDLE_RIGHT = "idleRight";

const string Hero::HERO_SLASH_ANIM_UP = "slashUp";
const string Hero::HERO_SLASH_ANIM_DOWN = "slashDown";
const string Hero::HERO_SLASH_ANIM_LEFT = "slashLeft";
const string Hero::HERO_SLASH_ANIM_RIGHT = "slashRight";

const string Hero::HERO_DASH_ANIM_UP = "dashUp";
const string Hero::HERO_DASH_ANIM_DOWN = "dashDown";
const string Hero::HERO_DASH_ANIM_LEFT = "dashLeft";
const string Hero::HERO_DASH_ANIM_RIGHT = "dashRight";

const string Hero::HERO_ANIM_DIE = "die";

const int Hero::HERO_STATE_IDLE = 0;
const int Hero::HERO_STATE_MOVE = 1;
const int Hero::HERO_STATE_SLASH = 2;
const int Hero::HERO_STATE_DASH = 3;
const int Hero::HERO_STATE_DEAD = 4;

Hero::Hero(AnimationSet* animSet) {
	this->animSet = animSet;
	type = "hero";

	//setup default hero values
	x = Globals::ScreenWidth / 2;
	y = Globals::ScreenHeight / 2;
	moveSpeed = 0;
	moveSpeedMax = 80;
	hp = hpMax = 20;
	damage = 0;
	collisionBoxW = 20;
	collisionBoxH = 24;
	collisionBoxYOffset = -20;
	direction = DIR_DOWN;
	honeydewQty = 3;

	changeAnimation(HERO_STATE_IDLE, true);
	updateCollisionBox();

	//Item* honeydewPotion = new Item(); // TODO: Inicializar item com os valores
	//honeydewPotion->id = 0;
	//honeydewPotion->quantity = honeydewQty;
	//honeydewPotion->name = "Honeydew Potion";
	//honeydewPotion->isOnGround = false;
	//addItemToInventory(honeydewPotion);
	//addItemToQuickAccess(0);
}

Hero::~Hero() {
}

void Hero::update() {
	// check if dead
	if (hp < 1 && state != HERO_STATE_DEAD) {
		changeAnimation(HERO_STATE_DEAD, true);
		moving = false;
		die();
	}

	if (hp > hpMax) {
		hp = hpMax;
	}

	updateCollisionBox();
	updateMovement();
	updateCollisions();
	updateHitBox();
	updateDamages();
	updateAnimation();
	updateInvincibleTimer();
}

void Hero::slash() {
	if (hp > 0 && (state == HERO_STATE_MOVE || state == HERO_STATE_IDLE)) {
		moving = false;
		frameTimer = 0;
		changeAnimation(HERO_STATE_SLASH, true);
		SoundManager::soundManager.playSound("swing");
	}
}

void Hero::dash() {
	if (hp > 0 && (state == HERO_STATE_MOVE || state == HERO_STATE_IDLE)) {
		moving = false;
		frameTimer = 0;

		//push the hero in the direction they are traveling
		slideAngle = angle;
		slideAmount = 200;
		invincibleTimer = 0.1;

		changeAnimation(HERO_STATE_DASH, true);

		SoundManager::soundManager.playSound("dash");
	}
}

void Hero::die() {
	moving = false;
	changeAnimation(HERO_STATE_DEAD, true);
}

void Hero::revive() {
	hp = hpMax;
	changeAnimation(HERO_STATE_IDLE, true);
	moving = false;
	x = Globals::ScreenWidth / 2;
	y = Globals::ScreenHeight / 2;
	slideAmount = 0;
}

void Hero::changeAnimation(int newState, bool resetFrameToBeginning, string animName) {
	state = newState;

	if (state == HERO_STATE_IDLE) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(HERO_ANIM_IDLE_DOWN);
		}
		else if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(HERO_ANIM_IDLE_UP);
		}
		else if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(HERO_ANIM_IDLE_LEFT);
		}
		else if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(HERO_ANIM_IDLE_RIGHT);
		}
	}
	else if (state == HERO_STATE_MOVE) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(HERO_ANIM_DOWN);
		}
		else if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(HERO_ANIM_UP);
		}
		else if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(HERO_ANIM_LEFT);
		}
		else if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(HERO_ANIM_RIGHT);
		}
	}
	else if (state == HERO_STATE_SLASH) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(HERO_SLASH_ANIM_DOWN);
		}
		else if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(HERO_SLASH_ANIM_UP);
		}
		else if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(HERO_SLASH_ANIM_LEFT);
		}
		else if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(HERO_SLASH_ANIM_RIGHT);
		}
	}
	else if (state == HERO_STATE_DASH) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(HERO_DASH_ANIM_DOWN);
		}
		else if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(HERO_DASH_ANIM_UP);
		}
		else if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(HERO_DASH_ANIM_LEFT);
		}
		else if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(HERO_DASH_ANIM_RIGHT);
		}
	}
	else if (state == HERO_STATE_DEAD) {
		currentAnim = animSet->getAnimation(HERO_ANIM_DIE);
	}

	if (resetFrameToBeginning) {
		currentFrame = currentAnim->getFrame(0);
	}
	else {
		currentFrame = currentAnim->getFrame(currentFrame->frameNumber);
	}
}

void Hero::updateAnimation() {
	if (currentFrame == NULL || currentAnim == NULL) {
		return;
	}

	if (state == HERO_STATE_MOVE && !moving) {
		changeAnimation(HERO_STATE_IDLE, true);
	}

	if (state != HERO_STATE_MOVE && moving) {
		changeAnimation(HERO_STATE_MOVE, true);
	}

	frameTimer += TimeController::timeController.dT;
	//change frames
	if (frameTimer >= currentFrame->duration) {
		//if we are at the end of animation
		if (currentFrame->frameNumber == currentAnim->getEndFrameNumber()) {
			if (state == HERO_STATE_SLASH || state == HERO_STATE_DASH) {
				//change back to moving state/anim
				changeAnimation(HERO_STATE_MOVE, true);
			}
			else if (state == HERO_STATE_DEAD && hp > 0) {
				//was dead but now have more hp, get back up
				changeAnimation(HERO_STATE_MOVE, true);
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

void Hero::updateDamages() {
	if (active && hp > 0 && invincibleTimer <= 0) {
		for (auto entity = Entity::entities.begin(); entity != Entity::entities.end(); entity++) {
			if ((*entity)->active && (*entity)->type == "enemy") {
				LivingEntity* enemy = (LivingEntity*)(*entity); //enemies are living entites

				/*if ((*entity)->animSet->imageName == "grob.png"){

				}*/

				if (enemy->damage > 0 && Entity::checkCollision(collisionBox, enemy->hitBox)) {
					enemy->hitLanded(this); // let attacker know they hit
					hp -= enemy->damage;

					if (hp > 0) {
						invincibleTimer = 0.3;
						SoundManager::soundManager.playSound("hit");
					}

					slideAngle = Entity::angleBetweenTwoEntities((*entity), this);
					slideAmount = 200;
				}
			}
		}
	}
}

bool Hero::isNearItem(Item* item){
	//float distance = distanceBetweenTwoEntities(this, item);
	float distance = distanceBetweenTwoPoints(x, y + (collisionBoxYOffset / 2), item->x, item->y);
	cout << "Distancia: " << distance << "\n";
	if (distance < 30.0) {
		cout << "PERTO DO ITEM!\n";
		return true;
	}
	return false;
}

void Hero::addItemToInventory(Item* item) {
	for (map<int, Item*>::iterator it = inventory.begin(); it != inventory.end(); it++) {
		if (it->first == item->id) {
			it->second->quantity++;
			cout << "Item qtd++: " << it->second->quantity << "\n";
			return;
		}
	}

	auto itemMap = std::make_pair(item->id, item);
	cout << "Adicionando item: " << itemMap.first << " " << itemMap.second << "\n";
	inventory.emplace(itemMap);
	cout << "Item inserido: " << itemMap.second->name << "\n";
}

void Hero::addItemToQuickAccess(int itemId) {
	quickAccessInventory[inventoryIndex] = itemId;
}

void Hero::useSelectedItemQuickAccess() {
	int id = quickAccessInventory[inventoryIndex];
	if (id < 0) {
		return;
	}

	useSelectedItem(id);
}

void Hero::useSelectedItem(int invIndex) {
	auto item = inventory.find(invIndex);
	if (item == inventory.end()) {
		cout << "Item na posicao " << invIndex << " n�o encontrado\n";
		return;
	}

	if (item->second->quantity == 0) {
		cout << "Item " << item->second->name << " zerado\n";
		return;
	}

	item->second->applyEffect(this);
	item->second->quantity--;
	cout << "Quantidade items " << item->second->name << ": " << item->second->quantity << "\n";
	if ((item->first != Item::HONEYDEW_POTION_ID) && (item->second->quantity <= 0)) {
		inventory.erase(item);
	}
}
