#include "wall.h"

Wall::Wall(int collisionBoxWidth, int collisionBoxHeight, int collisionBoxYOffset) {
	solid = true;
	this->type = "wall";
	collisionBoxW = collisionBoxWidth;
	collisionBoxH = collisionBoxHeight;
	this->collisionBoxYOffset = collisionBoxYOffset;

	updateCollisionBox();
	changeAnimation(0, false);
}

void Wall::update() {
	updateCollisionBox();
}
