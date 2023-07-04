#ifndef TERMITEMINER
#define TERMITEMINER

#include "livingEntity.h"

class TermiteMiner : public LivingEntity {
public:
	static const string TERMITE_MINER_ANIM_UP;
	static const string TERMITE_MINER_ANIM_DOWN;
	static const string TERMITE_MINER_ANIM_LEFT;
	static const string TERMITE_MINER_ANIM_RIGHT;
	static const string TERMITE_MINER_ANIM_IDLE_UP;
	static const string TERMITE_MINER_ANIM_IDLE_DOWN;
	static const string TERMITE_MINER_ANIM_IDLE_LEFT;
	static const string TERMITE_MINER_ANIM_IDLE_RIGHT;
	static const string TERMITE_MINER_ANIM_ATTACK_UP;
	static const string TERMITE_MINER_ANIM_ATTACK_DOWN;
	static const string TERMITE_MINER_ANIM_ATTACK_LEFT;
	static const string TERMITE_MINER_ANIM_ATTACK_RIGHT;
	static const string TERMITE_MINER_ANIM_DIE_UP;
	static const string TERMITE_MINER_ANIM_DIE_DOWN;
	static const string TERMITE_MINER_ANIM_DIE_LEFT;
	static const string TERMITE_MINER_ANIM_DIE_RIGHT;

	static const int TERMITE_MINER_STATE_IDLE;
	static const int TERMITE_MINER_STATE_MOVE;
	static const int TERMITE_MINER_STATE_ATTACK;
	static const int TERMITE_MINER_STATE_DEAD;

	static const int TERMITE_MINER_AI_NORMAL;
	static const int TERMITE_MINER_AI_CHASE;

	float thinkTimer = 0; //when 0, choose to do an action (AI)
	LivingEntity* target = NULL; //who should i chase

	int aiState = TERMITE_MINER_AI_NORMAL;

	TermiteMiner(AnimationSet* animSet);
	void update();
	void think();
	void attack();
	void die();
	void findNearestTarget();
	void changeAnimation(int newState, bool resetFrameToBeginning, string animName = "");
	void updateAnimation();
	void updateDamages();
	void populatePossibleDropItemsMap();
};

#endif // !TERMITEMINER
