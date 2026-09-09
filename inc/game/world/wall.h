#ifndef WALL
#define WALL

#include "entity.h"

class Wall : public Entity {
public:
	Wall(int collisionBoxWidth, int collisionBoxHeight, int collisionBoxYOffset);
	virtual ~Wall() = default;

	void update();
	void changeAnimation(int newState, bool resetFrameToBeginning, string animName = "") {
		//dont do anything
	}
	void updateCollisions() {
		//dont do anything
	}
};

#endif // !WALL

