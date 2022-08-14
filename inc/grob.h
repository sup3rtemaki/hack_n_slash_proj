#ifndef GROB
#define GROB

#include <cstdlib>
#include "livingEntity.h"
#include "soundManager.h"

class Grob : public LivingEntity {
public:
	static const string GROB_ANIM_UP;
	static const string GROB_ANIM_DOWN;
	static const string GROB_ANIM_LEFT;
	static const string GROB_ANIM_RIGHT;
	static const string GROB_ANIM_IDLE_UP;
	static const string GROB_ANIM_IDLE_DOWN;
	static const string GROB_ANIM_IDLE_LEFT;
	static const string GROB_ANIM_IDLE_RIGHT;
	static const string GROB_ANIM_ATTACK_UP;
	static const string GROB_ANIM_ATTACK_DOWN;
	static const string GROB_ANIM_ATTACK_LEFT;
	static const string GROB_ANIM_ATTACK_RIGHT;
	static const string GROB_ANIM_TELEGRAPH_UP;
	static const string GROB_ANIM_TELEGRAPH_DOWN;
	static const string GROB_ANIM_TELEGRAPH_LEFT;
	static const string GROB_ANIM_TELEGRAPH_RIGHT;
	static const string GROB_ANIM_DIE;

	static const int GROB_STATE_IDLE;
	static const int GROB_STATE_MOVE;
	static const int GROB_STATE_ATTACK;
	static const int GROB_STATE_TELEGRAPH;
	static const int GROB_STATE_DEAD;

	static const int GROB_AI_NORMAL;
	static const int GROB_AI_CHASE;

	static int grobsKilled;

	float thinkTimer = 0; //when 0, choose to do an action (AI)
	LivingEntity* target = NULL; //who should i chase

	int aiState = GROB_AI_NORMAL;

	Grob(AnimationSet* animSet);
	void update();
	void think();
	void telegraph();
	void attack();
	void die();
	void findNearestTarget();
	void changeAnimation(int newState, bool resetFrameToBeginning, string animName = "");
	void updateAnimation();
	void updateDamages();
	void populatePossibleDropItemsMap();

};

#endif // !GROB


