#include "entity.h"

const int Entity::DIR_UP = 0, Entity::DIR_DOWN = 1, Entity::DIR_LEFT = 2, Entity::DIR_RIGHT = 3, Entity::DIR_NONE = -1;

void Entity::update() { ; } // override me to do something
void Entity::draw() {
	//draws current frame
	if (currentFrame != NULL && animSet != NULL && active) {
		currentFrame->Draw(animSet->spriteSheet, x - Globals::camera.x, y - Globals::camera.y);
	}
	//draw collision box
	if (solid && Globals::debugging) {
		SDL_SetRenderDrawColor(Globals::renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawRect(Globals::renderer, &collisionBox);
		SDL_SetRenderDrawColor(Globals::renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(Globals::renderer, x, y);
	}
} 

void Entity::move(float angle) {
	moving = true;
	moveSpeed = moveSpeedMax;
	this->angle = angle;

	int newDirection = angleToDirection(angle);
	//if direction changed, update current animation
	if (direction != newDirection) {
		direction = newDirection;
		changeAnimation(state, false);
	}
}

void Entity::updateMovement() {
	updateCollisionBox();
	lastCollisionBox = collisionBox; //store collision box before move

	//resert total moves
	totalXMove = 0;
	totalYMove = 0;

	if (moveSpeed > 0) {
		// works out move distance using speed, deltaTime
		float moveDist = moveSpeed * (TimeController::timeController.dT) * moveLerp;
		if (moveDist > 0) {
			float xMove = moveDist * (cos(angle * Globals::PI / 180));
			float yMove = moveDist * (sin(angle * Globals::PI / 180));

			x += xMove;
			y += yMove;

			totalXMove = xMove;
			totalYMove = yMove;

			if (!moving) {
				//moveSpeed -= moveDist;
				moveSpeed -= moveDist * 3.0f;
			}
		}
	}

	if (slideAmount > 0) {
		float slideDist = slideAmount * TimeController::timeController.dT * moveLerp;
		if (slideDist > 0) {
			float xMove = slideDist * (cos(slideAngle * Globals::PI / 180));
			float yMove = slideDist * (sin(slideAngle * Globals::PI / 180));

			x += xMove;
			y += yMove;

			totalXMove += xMove;
			totalYMove += yMove;
			slideAmount -= slideDist * 2;
		}
		else {
			slideAmount = 0;
		}
	}
	updateCollisionBox();// move the collision box to where we are now
	SDL_UnionRect(&collisionBox, &lastCollisionBox, &collisionBox);

}
void Entity::updateCollisionBox() {
	collisionBox.x = x - collisionBox.w / 2;
	collisionBox.y = y + collisionBoxYOffset;
	collisionBox.w = collisionBoxW;
	collisionBox.h = collisionBoxH;
}

void Entity::updateCollisions() {
	if (active && collideWithSolids && (moveSpeed > 0 || slideAmount > 0)) {
		
		list<Entity*> collisions; //list of potential collisions

		//broad phase
		//if we collide with this entity
		for (auto entity = Entity::entities.begin(); entity != Entity::entities.end(); entity++) {
			
			if ((*entity)->active &&
				//(*entity)->type != this->type && //add this line if thing of same type cant collide
				(*entity)->solid &&
				Entity::checkCollision(collisionBox, (*entity)->collisionBox)) {

				collisions.push_back(*entity); // add it to the list
			}
		}
		// if we have a list of potential collisions, check them
		if (collisions.size() > 0) {
			updateCollisionBox();

			float collisionTime = 1; //1 = no collision
			float normalX, normalY; //wich side we collide into stuff

			//collision before we try to move
			SDL_Rect startingCollisionBox = lastCollisionBox;

			//loop through entities that are in the list from broad phase
			for (auto entity = collisions.begin(); entity != collisions.end(); entity++) {
				//temp var for normal x and y and temp collisionTime
				float tmpNormalX, tmpNormalY;
				float tmpCollisionTime = SweptAABB(startingCollisionBox, totalXMove, totalYMove, (*entity)->collisionBox, tmpNormalX, tmpNormalY);

				//tmpCollisionTime < lastCollisionTime, use it instead
				if (tmpCollisionTime < collisionTime) {
					collisionTime = tmpCollisionTime;
					normalX = tmpNormalX;
					normalY = tmpNormalY;
				}
			}

			//if there was a collision, slide off of it
			if (collisionTime < 1.0f) {
				//if die on solids, run crash function
				if (dieOnSolids) {
					crashOntoSolid();
				}

				//move our collisionBox position to where we collided
				startingCollisionBox.x += totalXMove * collisionTime;
				startingCollisionBox.y += totalYMove * collisionTime;

				//how much move time was remaining
				float remainingTime = 1.0f - collisionTime;

				//update entities x and y to here we bumped into other entity
				x = startingCollisionBox.x + startingCollisionBox.w / 2;
				y = startingCollisionBox.y - collisionBoxYOffset;

				//collision response: slide
				//work out dotProduct using remainingTime
				float dotProd = (totalXMove * normalY + totalYMove * normalX) * remainingTime;
				totalXMove = dotProd * normalY;
				totalYMove = dotProd * normalX;
				x += totalXMove;
				y += totalYMove;

				//store coliisionbox at this point
				lastCollisionBox = startingCollisionBox;

				//update entities main collisionbox
				updateCollisionBox();

				//sliding may have bumped into other objects: run function again
				updateCollisions();
			}
		}
	}
}

void Entity::dropItem(int itemId, int quant, int xPos, int yPos){
	dropItemFlag = true;
	dropItemId = itemId;
	dropItemXPos = xPos;
	dropItemYPos = yPos;
	dropItemQty = quant;
}

void Entity::checkIfDropsItem(){
	if (possibleDropItemsMap.empty()) {
		return;
	}

	int dropChance = rand() % 100;
	int itemId = -1;
	int quant = -1;

	for (auto it = possibleDropItemsMap.begin(); it != possibleDropItemsMap.end(); it++) {
		if ((dropChance >= it->second.first.first) && (dropChance < it->second.first.second)) {
			itemId = it->first;
			quant = 1 + rand() % it->second.second;
		}
	}

	if (itemId < 0) {
		return;
	}

	if (quant < 1) {
		return;
	}

	dropItem(itemId, quant, x, y);
}

// help functions
//return type: gives 0 -1 depending on where collision is. 1 = no collision, 0 = collide immediately, 0.5 = halfway through, etc
//params: movingBox = entity being checked
//		vx, vy = velocities moving box is moving
//		otherbox = other entities collision box we may collide
//		normalX, normalY = wich side of otherbox we collided with (pass by reference)
float Entity::SweptAABB(SDL_Rect movingBox, float vx, float vy, SDL_Rect otherBox, float& normalX, float& normalY) {
	float xInvEntry, yInvEntry;
	float xInvExit, yInvExit;

	//find distance between objects on near and far sides fot x and y
	if (vx > 0.0f) {
		xInvEntry = otherBox.x - (movingBox.x + movingBox.w);
		xInvExit = (otherBox.x + otherBox.w) - movingBox.x;
	}
	else {
		xInvEntry = (otherBox.x + otherBox.w) - movingBox.x;
		xInvExit = otherBox.x - (movingBox.x + movingBox.w);
	}

	if (vy > 0.0f) {
		yInvEntry = otherBox.y - (movingBox.y + movingBox.h);
		yInvExit = (otherBox.y + otherBox.h) - movingBox.y;
	}
	else {
		yInvEntry = (otherBox.y + otherBox.h) - movingBox.y;
		yInvExit = otherBox.y - (movingBox.y + movingBox.h); 
	}

	//find time of collision and time of leaving for each axis
	float xEntry, yEntry;
	float xExit, yExit;

	if (vx == 0.0f) {
		xEntry = -std::numeric_limits<float>::infinity();
		xExit = std::numeric_limits<float>::infinity();
	}
	else {
		xEntry = xInvEntry / vx;
		xExit = xInvExit / vx;
	}

	if (vy == 0.0f) {
		yEntry = -std::numeric_limits<float>::infinity();
		yExit = std::numeric_limits<float>::infinity();
	}
	else {
		yEntry = yInvEntry / vy;
		yExit = yInvExit / vy;
	}

	//find the earliest/latest times of collision
	float entryTime = std::max(xEntry, yEntry);
	float exitTime = std::min(xExit, yExit);

	//if there was no collision
	if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f || xEntry > 1.0f || yEntry > 1.0f) {
		normalX = 0.0f;
		normalY = 0.0f;
		return 1.0f;
	}
	else {
		//there was a collision
		//work out wich sides/normal of otherbox we hit
		if (xEntry > yEntry) {
			//we hit on x axis
			if (xInvEntry < 0.0f) {
				normalX = 1;//hit right
				normalY = 0;//not hit top or bottom of otherbox
			}
			else {
				normalX = -1;//hit left
				normalY = 0;//not hit top or bottom of otherbox
			}
		}
		else {
			//we hit on y axis
			if (yInvEntry < 0.0f) {
				normalX = 0;
				normalY = 1;
			}
			else {
				normalX = 0;
				normalY = -1;
			}
		}

		//return the time of collision (0 - 1)
		return entryTime;
	}
}

float Entity::distanceBetweenTwoRects(SDL_Rect& r1, SDL_Rect& r2) {
	SDL_Point e1, e2;

	e1.x = r1.x + r1.w / 2;
	e1.y = r1.y + r1.h / 2;

	e2.x = r2.x + r2.w / 2;
	e2.y = r2.y + r2.h / 2;

	float d = abs(sqrt(pow(e2.x - e1.x, 2) + pow(e2.y - e1.y, 2)));
	return d;
}

float Entity::distanceBetweenTwoEntities(Entity* e1, Entity* e2) {
	float d = abs(sqrt(pow(e2->x - e1->x, 2) + pow(e2->y - e1->y, 2)));
	return d;
}

float Entity::distanceBetweenTwoPoints(float cx1, float cy1, float cx2, float cy2) {
	float d = abs(sqrt(pow(cx2 - cx1, 2) + pow(cy2 - cy1, 2)));
	return d;
}

float Entity::angleBetweenTwoEntities(Entity* e1, Entity* e2) {
	float dx, dy;
	float x1 = e1->x, y1 = e1->y, x2 = e2->x, y2 = e2->y;

	dx = x2 - x1;
	dy = y2 - y1;

	return atan2(dy, dx) * 180 / Globals::PI;
}

bool Entity::checkCollision(SDL_Rect cbox1, SDL_Rect cbox2) {
	SDL_Rect intersection;
	if (SDL_IntersectRect(&cbox1, &cbox2, &intersection)) {
		return true;
	}
	
	return false;
}

int Entity::angleToDirection(float angle) {
	if ((angle >= 0 && angle <= 45) || (angle >= 315 && angle <= 360)) {
		return DIR_RIGHT;
	}
	else if (angle >= 45 && angle <= 135) {
		return DIR_DOWN;
	}
	else if (angle >= 135 && angle <= 225) {
		return DIR_LEFT;
	}
	else {
		return DIR_UP;
	}
}

float Entity::angleBetweenTwoPoints(float cx1, float cy1, float cx2, float cy2) {
	float dx = cx2 - cx1;
	float dy = cy2 - cy1;

	return atan2(dy, dx) * 180 / Globals::PI;
}

float Entity::angleBetweenTwoRects(SDL_Rect& r1, SDL_Rect& r2) {
	float cx1 = r1.x + (r1.w / 2);
	float cy1 = r1.y + (r1.h / 2);

	float cx2 = r2.x + (r2.w / 2);
	float cy2 = r2.y + (r2.h / 2);

	return angleBetweenTwoPoints(cx1, cy1, cx2, cy2);
}

list<Entity*> Entity::entities;

bool Entity::EntityCompare(const Entity* const& a, const Entity* const& b) {
	if (a != 0 && b != 0) {
		return ((a->y) < (b->y));
	}
	else {
		return false;
	}
}

void Entity::removeInactiveEntitiesFromList(list<Entity*>* entityList, bool deleteEntities) {
	for (auto entity = entityList->begin(); entity != entityList->end();) {
		if (!(*entity)->active) {
			if (deleteEntities) {
				delete (*entity);
			}
			entity = entityList->erase(entity);
		}
		else {
			entity++;
		}
	}
}

void Entity::removeAllFromList(list<Entity*>* entityList, bool deleteEntities) {
	for (auto entity = entityList->begin(); entity != entityList->end();) {
		if (deleteEntities) {
			delete (*entity);
		}
		entity = entityList->erase(entity);
		
	}
}
