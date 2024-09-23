#ifndef ENEMYENTITY
#define ENEMYENTITY

#include "livingEntity.h"

class EnemyEntity : public LivingEntity {
public:
	SDL_Point currentTargetPos;
	int pheromoneTrailIndex = 0;
	bool isChasingPheromone = false;

	virtual void updateDamages() = 0; // how we get damaged by other things
	virtual void die() = 0;
	virtual void hitLanded(LivingEntity* entity) { ; }
};

#endif // !ENEMYENTITY
