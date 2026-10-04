#ifndef ENEMYENTITY
#define ENEMYENTITY

#include "livingEntity.h"
#include "lootDropSource.h"
#include "gameKillStats.h"

class EnemyEntity : public LivingEntity, public LootDropSource {
public:
	GameKillStats* gameKillStats = nullptr;
	SDL_Point currentTargetPos;
	int pheromoneTrailIndex = 0;
	bool isChasingPheromone = false;

	virtual void updateDamages() = 0; // how we get damaged by other things
	virtual void die() = 0;
	virtual void hitLanded(LivingEntity* entity) { ; }
	virtual void pursueTarget(LivingEntity* entity) { ; }
	void setGameKillStats(GameKillStats* stats) { gameKillStats = stats; }
};

#endif // !ENEMYENTITY
