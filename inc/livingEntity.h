#ifndef LIVINGENTITY
#define LIVINGENTITY

#include "entity.h"

#include <deque>

//abstract class
class LivingEntity : public Entity {
public:
	static const float TIME_TICK;

	float hp;
	float hpMax;
	float stamina;
	float staminaMax;
	int damage = 0;
	int enemyId;
	SDL_Rect hitBox;
	float invincibleTimer = 0;
	float distanceThreshold;

	float healStatusTimer;
	float healStatusTimerAux;
	float healRate;
	float healStatusAmount;

	float staminaStatusTimer;
	float staminaRate;
	float staminaStatusAmount;

	deque<SDL_Point> pheromoneTrail;
	SDL_Point currentTargetPos;
	int pheromoneTrailIndex = 0;

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

