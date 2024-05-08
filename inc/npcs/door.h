#ifndef DOOR
#define DOOR

#include "entity.h"
class Door : public Entity {
public:
	static const int DOOR_STATE_CLOSED;
	static const int DOOR_STATE_OPENING;
	static const int DOOR_STATE_OPEN;

	static const string DOOR_CLOSED_ANIM_SUFFIX;
	static const string DOOR_OPENING_ANIM_SUFFIX;
	static const string DOOR_OPEN_ANIM_SUFFIX;

	string animPrefix;
	bool isClosed;
	bool isLocked;

	Door(AnimationSet* animSet, string prefix, bool isClosed, int posX, int posY, int width, int height, int collisionBoxYOffset);

	void update();
	void changeAnimation(int newState, bool resetFrameToBeginning, string animName = "");
	void updateAnimation();
	void updateCollisionBox();
	void openDoor();
};

#endif // !DOOR
