#include "entity.h"

const int Entity::DIR_UP = 0, Entity::DIR_DOWN = 1, Entity::DIR_LEFT = 2, Entity::DIR_RIGHT = 3, Entity::DIR_NONE = -1;

void Entity::update() { ; } // override me to do something
void Entity::draw() {
	//draws current frame
	if (currentFrame != NULL && active) {
		currentFrame->Draw(animSet->spriteSheet, x, y);
	}
	//draw collision box
	if (solid && Globals::debugging) {
		SDL_SetRenderDrawColor(Globals::renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawRect(Globals::renderer, &collisionBox);
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

	//resert totasl moves
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
				moveSpeed -= moveDist;
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
			slideAmount -= slideDist;
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

		//if we collide with this entity
		for (auto entity = Entity::entities.begin(); entity != Entity::entities.end(); entity++) {
			
			if ((*entity)->active &&
				(*entity)->type != this->type &&
				(*entity)->solid &&
				Entity::checkCollision(collisionBox, (*entity)->collisionBox)) {

				collisions.push_back(*entity); // add it to the list
			}
		}
		// if we have a list of potential collisions, check them
		if (collisions.size() > 0) {
			updateCollisionBox();

			//multisample check for collisions
			//find the sample distance we should travel between checks
			float boxTravelSize = 0;
			if (collisionBox.w < collisionBox.h) {
				boxTravelSize = collisionBox.w / 4;
			}
			else {
				boxTravelSize = collisionBox.h / 4;
			}

			// use sampleBox to check for collisions from start point to end point
			SDL_Rect sampleBox = lastCollisionBox;
			float collisionMoveX = sampleBox.x, collisionMoveY = sampleBox.y;
			float movementAngle = Entity::angleBetweenTwoRects(lastCollisionBox, collisionBox);
			bool foundCollision = false;
			while (!foundCollision) {
				//check samplebox for collisions where it is now
				SDL_Rect intersection;
				for (auto entity = collisions.begin(); entity != collisions.end(); entity++) {
					if (SDL_IntersectRect(&sampleBox, &(*entity)->collisionBox, &intersection)) {
						moveSpeed = 0;
						moving = false;
						foundCollision = true;
						slideAngle = angleBetweenTwoEntities((*entity), this);

						//intersecting a entity, need to do collision resolution
						if (intersection.w < intersection.h) {
							if (lastCollisionBox.x + lastCollisionBox.w / 2 < (*entity)->collisionBox.x + (*entity)->collisionBox.w / 2) {
								sampleBox.x -= intersection.w; // started on left, so move left out of collision
							}
							else {
								sampleBox.x += intersection.w; // started on right, so move right out of collision
							}
						}
						else {
							if (lastCollisionBox.y + lastCollisionBox.h / 2 < (*entity)->collisionBox.y + (*entity)->collisionBox.h / 2) {
								sampleBox.y -= intersection.h; // started on top, so move up out of collision
							}
							else {
								sampleBox.y += intersection.h; // started on bottom, so move down out of collision
							}
						}
					}
				}

				//if collisionsfound or sampleBox is at same place as collisionBox, exit loop
				if (foundCollision || (sampleBox.x == collisionBox.x && sampleBox.y == collisionBox.y)) {
					break;
				}

				//move sample box up to check the next spot
				if (distanceBetweenTwoRects(sampleBox, collisionBox) > boxTravelSize) {
					movementAngle = Entity::angleBetweenTwoRects(sampleBox, collisionBox);
					float xMove = boxTravelSize * (cos(movementAngle * Globals::PI / 180));
					float yMove = boxTravelSize * (sin(movementAngle * Globals::PI / 180));

					//fix for float values below 1
					collisionMoveX += xMove;
					collisionMoveY += yMove;
					sampleBox.x = collisionMoveX;
					sampleBox.y = collisionMoveY;
				}
				else {
					sampleBox = collisionBox;
				}
			}

			if (foundCollision) {
				//move our entity to where the sampleBox ended
				slideAmount = slideAmount / 2;
				x = sampleBox.x + sampleBox.w / 2;
				y = sampleBox.y - collisionBoxYOffset;
			}

			updateCollisionBox();
		}
	}
}

// help functions
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
