#ifndef CHECKPOINT
#define CHECKPOINT

#include "entity.h"
#include <memory>

class Checkpoint : public Entity {
public:
	static const string CHECKPOINT_ANIM_ACTIVE;
	static const string CHECKPOINT_ANIM_INACTIVE;

	static const int CHECKPOINT_STATE_ACTIVE;
	static const int CHECKPOINT_STATE_INACTIVE;


	bool isActivated;
	string mapFileName;

	Checkpoint(int id = 0, const string& mapFileName = "");
	virtual ~Checkpoint() = default;

	void activate();

private:
	std::unique_ptr<AnimationSet> localAnimSet;
	void update();
	void updateAnimation();
	void changeAnimation(int newState, bool resetFrameToBeginning, string animName = "");
};

#endif // !CHECKPOINT
