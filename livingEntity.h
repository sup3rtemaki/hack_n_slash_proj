#ifndef LIVINGENTITY
#define LIVINGENTITY

#include "entity.h"

//abstract class
class LivingEntity : public Entity {
public:
	int hp, hpMax;
	int damage = 0;
	SDL_Rect hitBox;
	float invincibleTimer = 0;

	virtual void updateHitBox();
	virtual void updateDamages() = 0; // how we get damaged by other things
	virtual void die() = 0;
	virtual void updateInvincibleTimer();

	void draw(); // override entites draw()
};

#endif // !LIVINGENTITY

