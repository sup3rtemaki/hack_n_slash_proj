#ifndef WALL
#define WALL

#include "entity.h"

class Wall : public Entity {
public:
	Wall(AnimationSet* animSet);
	void update();
	void changeAnimation(int newState, bool resetFrameToBeginning, string animName = "");
	void updateCollisions() {
		//dont do anything
	}
};

#endif // !WALL

