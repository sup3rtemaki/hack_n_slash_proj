#ifndef BULLET
#define BULLET

#include <cstdlib>
#include "livingEntity.h"
#include "soundManager.h"

class Bullet : public LivingEntity {
public:
	static const string BULLET_ANIM_BULLET;

	Bullet(AnimationSet* animSet, int x, int y);
	void update();
	void die() {};
	void changeAnimation(int newState, bool resetFrameToBeginning, string animName = "");
	void updateAnimation();
	void updateDamages() { ; }
	void hitLanded(LivingEntity* entity);
	virtual void crashOntoSolid();
};

#endif // !BULLET

