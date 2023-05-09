#ifndef LIVINGENTITY
#define LIVINGENTITY

#include "entity.h"

//abstract class
class LivingEntity : public Entity {
public:
	static const float TIME_TICK;

	float hp, hpMax;
	int damage = 0;
	int enemyId;
	SDL_Rect hitBox;
	float invincibleTimer = 0;

	float healStatusTimer;
	float healStatusTimerAux;
	float healRate;
	float healStatusAmount;

	virtual void updateHitBox();
	virtual void updateDamages() = 0; // how we get damaged by other things
	virtual void die() = 0;
	virtual void updateInvincibleTimer();
	virtual void hitLanded(LivingEntity* entity) { ; }

	void draw(); // override entites draw()
	void changeAnimation(int newState, bool resetFrameToBeginning, string animName = "");

	static void saveDeadEnemiesIds(list<Entity*> entityList, list<int>& enemiesIdList);
};

#endif // !LIVINGENTITY

