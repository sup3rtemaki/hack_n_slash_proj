#ifndef CHECKPOINT
#define CHECKPOINT

#include "entity.h"

class Checkpoint : public Entity {
public:
	static const string CHECKPOINT_ANIM_ACTIVE;
	static const string CHECKPOINT_ANIM_INACTIVE;

	static const int CHECKPOINT_STATE_ACTIVE;
	static const int CHECKPOINT_STATE_INACTIVE;

	bool isActivated;
	string mapFileName;

	Checkpoint(AnimationSet* animSet, int id, string& mapFileName);

	void activate();

private:
	void update();
	void updateAnimation();
	void changeAnimation(int newState, bool resetFrameToBeginning, string animName = "");
};

#endif // !CHECKPOINT
