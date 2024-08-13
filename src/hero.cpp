#include "hero.h"

#include "checkpoint.h"
#include "door.h"
#include "ui/actionMessageUi.h"

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

const string Hero::HERO_CONSUMING_ANIM_UP = "consumeItemUp";
const string Hero::HERO_CONSUMING_ANIM_DOWN = "consumeItemDown";
const string Hero::HERO_CONSUMING_ANIM_LEFT = "consumeItemLeft";
const string Hero::HERO_CONSUMING_ANIM_RIGHT = "consumeItemRight";

const string Hero::HERO_SHOOT_ANIM_UP = "shootUp";
const string Hero::HERO_SHOOT_ANIM_DOWN = "shootDown";
const string Hero::HERO_SHOOT_ANIM_LEFT = "shootLeft";
const string Hero::HERO_SHOOT_ANIM_RIGHT = "shootRight";

const string Hero::HERO_ANIM_DIE = "die";

const int Hero::HERO_STATE_IDLE = 0;
const int Hero::HERO_STATE_MOVE = 1;
const int Hero::HERO_STATE_SLASH = 2;
const int Hero::HERO_STATE_DASH = 3;
const int Hero::HERO_STATE_DEAD = 4;
const int Hero::HERO_STATE_CONSUMING_ITEM = 5;
const int Hero::HERO_STATE_SHOOTING = 6;
const int Hero::HERO_STATE_RESTING = 7;

Hero::Hero(AnimationSet* animSet) {
	this->animSet = animSet;
	type = "hero";

	//setup default hero values
	x = Globals::ScreenWidth / 2;
	y = Globals::ScreenHeight / 2;
	moveSpeed = 0;
	moveSpeedMax = 50;
	hp = hpMax = 100;
	stamina = staminaMax = 50;
	staminaStatusAmount = 0.3f;
	damage = 0;
	collisionBoxW = 20;
	collisionBoxH = 24;
	collisionBoxYOffset = -20;
	direction = DIR_DOWN;
	honeydewQty = 3;
	inventoryIndex = 0;
	nearestDoor = nullptr;
	nearestCheckpoint = nullptr;
	changeAnimation(HERO_STATE_IDLE, true);
	updateCollisionBox();
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

	if (stamina < 0.f) {
		stamina = 0;
	}

	statusTimerTick();
	updateCollisionBox();
	updateMovement();
	updateCollisions();
	updateHitBox();
	updateDamages();
	updateAnimation();
	updateInvincibleTimer();
}

void Hero::slash() {
	if (hp > 0 &&
		(state == HERO_STATE_MOVE || state == HERO_STATE_IDLE) &&
		stamina > 0.f) {
		moving = false;
		frameTimer = 0;
		stamina -= 17.f;
		changeAnimation(HERO_STATE_SLASH, true);
		SoundManager::soundManager.playSound("swing");
	}
}

void Hero::dash() {
	if (hp > 0 &&
		(state == HERO_STATE_MOVE || state == HERO_STATE_IDLE) &&
		stamina > 0.f) {
		moving = false;
		frameTimer = 0;

		//push the hero in the direction they are traveling
		slideAngle = angle;
		slideAmount = 200;
		invincibleTimer = 0.1;
		stamina -= 30.f;

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
	stamina = staminaMax;
	changeAnimation(HERO_STATE_IDLE, true);
	moving = false;
	x = lastCheckpointPos.x;
	y = lastCheckpointPos.y;
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
	else if (state == HERO_STATE_CONSUMING_ITEM) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(HERO_CONSUMING_ANIM_DOWN);
		}
		else if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(HERO_CONSUMING_ANIM_UP);
		}
		else if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(HERO_CONSUMING_ANIM_LEFT);
		}
		else if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(HERO_CONSUMING_ANIM_RIGHT);
		}
	}
	else if (state == HERO_STATE_SHOOTING) {
		if (direction == DIR_DOWN) {
			currentAnim = animSet->getAnimation(HERO_SHOOT_ANIM_DOWN);
		}
		else if (direction == DIR_UP) {
			currentAnim = animSet->getAnimation(HERO_SHOOT_ANIM_UP);
		}
		else if (direction == DIR_LEFT) {
			currentAnim = animSet->getAnimation(HERO_SHOOT_ANIM_LEFT);
		}
		else if (direction == DIR_RIGHT) {
			currentAnim = animSet->getAnimation(HERO_SHOOT_ANIM_RIGHT);
		}
	}
	else if (state == HERO_STATE_RESTING) { // TODO: Criar anims para descanso na fogueira
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

	if (state != HERO_STATE_MOVE && state != HERO_STATE_CONSUMING_ITEM && moving) {
		changeAnimation(HERO_STATE_MOVE, true);
	}

	if (state == HERO_STATE_RESTING) {
		// TODO: Fazer anim e lógica de sentar na fogueira depois de implementar os menus
		state = HERO_STATE_IDLE;
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
			else if (state == HERO_STATE_CONSUMING_ITEM || state == HERO_STATE_SHOOTING) {
				changeAnimation(HERO_STATE_IDLE, true);
			}
			else {
				// reset animation (loops it)
				currentFrame = currentAnim->getFrame(0);
			}
		}
		else {
			if (state == HERO_STATE_CONSUMING_ITEM) {
				moving = false;
			}

			// just move to next frame in this animation
			currentFrame = currentAnim->getNextFrame(currentFrame);
		}

		frameTimer = 0;
	}
}

void Hero::updateDamages() {
	if (active && hp > 0 && invincibleTimer <= 0) {
		for (auto entity = Entity::entities.begin(); entity != Entity::entities.end(); entity++) {
			if ((*entity)->active && ((*entity)->type == "enemy" || (*entity)->type == "boss")) {
				LivingEntity* enemy = (LivingEntity*)(*entity); //enemies are living entites

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

void Hero::takeAction() {
	if (nearestDoor != nullptr) {
		if (!nearestDoor->isLocked) {
			openDoor();
		}
		else {
			actionMessageUi->setMessage("Door Locked");
			actionMessageUi->lock();
		}
	}
	else if (nearestCheckpoint != nullptr) {
		lastCheckpointPos.x = nearestCheckpoint->x;
		lastCheckpointPos.y = nearestCheckpoint->y;
		lastCheckpointMapFile = nearestCheckpoint->mapFileName;

		if (nearestCheckpoint->isActivated) {
			rest();
		}
		else {
			nearestCheckpoint->activate();
			isCheckpointActivatedFlag = true;
		}
	}
	else {
		pickNearItemFromGround();
	}
}

void Hero::addItemToInventory(Item* item) {
	addedItemName = item->name;
	qtyItemsPicked = item->quantity;

	for (map<int, Item*>::iterator it = inventory.begin(); it != inventory.end(); it++) {
		if (it->first == item->id) {
			it->second->quantity += item->quantity;
			cout << "Item qtd++: " << it->second->quantity << "\n";
			return;
		}
	}

	auto itemMap = std::make_pair(item->id, item);
	cout << "Adicionando item: " << itemMap.first << " " << itemMap.second << "\n";
	inventory.insert(itemMap);
	cout << "Item inserido: " << itemMap.second->name << "\n";
}

void Hero::addItemToQuickAccess(int itemId, int position) {
	if (position < 0 || position >= quickAccessInventory.size()) {
		position = 0;
		for (position; position < quickAccessInventory.size(); position++) {
			if (quickAccessInventory[position] == itemId) { // Item ja existe no quickAccessInventory
				return;
			}
			if (quickAccessInventory[position] == -1) { // Achou proxima posição valida
				break; // Sai do loop pra guardar a posição
			}
		}
	}

	if (position == quickAccessInventory.size()) { // quickAccessInventory está cheio
		position = 0;
	}

	quickAccessInventory[position] = itemId;
}

void Hero::useSelectedItemQuickAccess() {
	int id = quickAccessInventory[quickAccessInventoryIndex];
	if (id < 0) {
		cout << "ID < 0!" << id << "\n";
		return;
	}

	useSelectedItem(id);
}

void Hero::pickNearItemFromGround() {
	if (nearItems.empty()) {
		cout << "Nenhum item proximo!\n";
		return;
	}

	findNearestItem();

	if (currentNearItem == nullptr) {
		cout << "currentNearItem nulo!\n";
		return;
	}
	currentNearItem->active = false;
	currentNearItem->isOnGround = false;
	addItemToInventory(currentNearItem);
	addItemToQuickAccess(currentNearItem->id);
	int cont = 0;
	for (auto const& i : currentMap->itemsInMap) {
		cout << "Dentro for hero ";
		cout << i.first << " " 
			<< (currentNearItem->id == get<0>(i.second)) << " " 
			<< currentNearItem->id << " - " << get<0>(i.second) << " "
			<< (currentNearItem->x == get<1>(i.second)) << " " 
			<< currentNearItem->x << " - " << get<1>(i.second) << " "
			<< (currentNearItem->y == get<2>(i.second)) << " "
			<< currentNearItem->y << " - " << get<2>(i.second) << "\n";
		if (!i.first && 
			currentNearItem->id == get<0>(i.second) &&
			currentNearItem->x == get<1>(i.second) &&
			currentNearItem->y == get<2>(i.second)) {
			cout << "-Achou item hero\n";
			currentMap->itemsInMap[cont].first = true;
			break;
		}
		else {
			cont++;
		}
	}
	
	currentNearItem = nullptr;
}

void Hero::rest() {
	changeAnimation(HERO_STATE_RESTING, true);
	hp = hpMax;
	stamina = staminaMax;
	mustSaveGame = true;
	isRested = true;
}

void Hero::statusTimerTick() {
	if (stamina <= staminaMax) {
		stamina += staminaStatusAmount;
	}

	healTimerTick();
}

void Hero::openDoor() {
	if (nearestDoor == nullptr) return; // Safety for key item

	nearestDoor->open();
}

void Hero::healTimerTick() {
	if (healStatusTimer <= 0) {
		healStatusTimer = 0;
		healStatusAmount = 0;
	}
	else {
		hp += healStatusAmount;
		healStatusTimer -= TimeController::timeController.dT;
	}
}

void Hero::findNearestItem() {
	if (nearItems.size() == 1) {
		cout << "So um " << (*nearItems.begin())->x << " " << (*nearItems.begin())->y << "\n";
		currentNearItem = (*nearItems.begin());
		cout << currentNearItem->x << " " << currentNearItem->y << "\n";
		return;
	}

	int prevDistance = 40;
	int distance;
	for (auto const& i : nearItems) {
		distance = distanceBetweenTwoPoints(x, y + (collisionBoxYOffset / 2), i->x, i->y);
		if (distance <= prevDistance){
			currentNearItem = i;
			prevDistance = distance;
		}
	}

	nearItems.remove(currentNearItem);
}

void Hero::useSelectedItem(int invIndex) {
	if (hp > 0 && (state == HERO_STATE_MOVE || state == HERO_STATE_IDLE)) {
		auto item = inventory.find(invIndex);
		if (item == inventory.end()) {
			cout << "Item na posicao " << invIndex << " não encontrado\n";
			return;
		}

		if (item->second->quantity == 0) {
			cout << "Item " << item->second->name << " zerado\n";
			return;
		}

		item->second->applyEffect(dynamic_cast<LivingEntity*>((this)));
		item->second->quantity--;
		cout << "Quantidade items " << item->second->name << ": " << item->second->quantity << "\n";
		if ((item->first == -1) || (item->second->quantity <= 0)) {
			//inventory.erase(item);
			return;
		}

		moving = false;
		frameTimer = 0;
		if (item->second->type == "cProjectileItem") {
			changeAnimation(HERO_STATE_SHOOTING, true);
		}
		else if (item->second->type == "kKeyItem") {
			// TODO: Criar anim pra abrir porta
			changeAnimation(HERO_STATE_SHOOTING, true);
		}
		else {
			changeAnimation(HERO_STATE_CONSUMING_ITEM, true);
		}
	}
}
