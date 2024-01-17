#ifndef STONEPROJECTILE
#define STONEPROJECTILE

#include <cstdlib>
#include "livingEntity.h"

class StoneProjectile : public LivingEntity {
public:
	static const string STONE_PROJECTILE_ANIM_THROW_UP_LEFT;
	static const string STONE_PROJECTILE_ANIM_THROW_DOWN_RIGHT;
	static const string STONE_PROJECTILE_ANIM_DESTROY;

	static const int STONE_PROJECTILE_STATE_UP_LEFT;
	static const int STONE_PROJECTILE_STATE_DOWN_RIGHT;
	static const int STONE_PROJECTILE_STATE_DESTROY;

	float lifetime;
	float lifetimeTimer;

	StoneProjectile(AnimationSet* animSet, int x, int y);
	void update();
	void die();
	void changeAnimation(int newState, bool resetFrameToBeginning, string animName = "");
	void updateAnimation();
	void updateDamages() { ; }
	void hitLanded(LivingEntity* entity);
	virtual void crashOntoSolid();
};

#endif // !STONEPROJECTILE
