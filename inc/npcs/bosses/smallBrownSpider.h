#ifndef SMALLBROWNSPIDER
#define SMALLBROWNSPIDER

#include "entities/enemyEntity.h"
class SmallBrownSpider : public EnemyEntity {
public:
	// animations
	static const string SMALL_BROWN_SPIDER_ANIM_UP;
	static const string SMALL_BROWN_SPIDER_ANIM_DOWN;
	static const string SMALL_BROWN_SPIDER_ANIM_LEFT;
	static const string SMALL_BROWN_SPIDER_ANIM_RIGHT;

	static const string SMALL_BROWN_SPIDER_ANIM_IDLE_UP;
	static const string SMALL_BROWN_SPIDER_ANIM_IDLE_DOWN;
	static const string SMALL_BROWN_SPIDER_ANIM_IDLE_LEFT;
	static const string SMALL_BROWN_SPIDER_ANIM_IDLE_RIGHT;

	static const string SMALL_BROWN_SPIDER_ANIM_TELEGRAPH_DASH_UP;
	static const string SMALL_BROWN_SPIDER_ANIM_TELEGRAPH_DASH_DOWN;
	static const string SMALL_BROWN_SPIDER_ANIM_TELEGRAPH_DASH_LEFT;
	static const string SMALL_BROWN_SPIDER_ANIM_TELEGRAPH_DASH_RIGHT;

	static const string SMALL_BROWN_SPIDER_ANIM_DASH_UP;
	static const string SMALL_BROWN_SPIDER_ANIM_DASH_DOWN;
	static const string SMALL_BROWN_SPIDER_ANIM_DASH_LEFT;
	static const string SMALL_BROWN_SPIDER_ANIM_DASH_RIGHT;

	static const string SMALL_BROWN_SPIDER_ANIM_SLASH_RHAND_UP;
	static const string SMALL_BROWN_SPIDER_ANIM_SLASH_LHAND_UP;
	static const string SMALL_BROWN_SPIDER_ANIM_SLASH_RHAND_DOWN;
	static const string SMALL_BROWN_SPIDER_ANIM_SLASH_LHAND_DOWN;
	static const string SMALL_BROWN_SPIDER_ANIM_SLASH_RHAND_LEFT;
	static const string SMALL_BROWN_SPIDER_ANIM_SLASH_LHAND_LEFT;
	static const string SMALL_BROWN_SPIDER_ANIM_SLASH_RHAND_RIGHT;
	static const string SMALL_BROWN_SPIDER_ANIM_SLASH_LHAND_RIGHT;

	static const string SMALL_BROWN_SPIDER_ANIM_DIE;

	// states
	static const int SMALL_BROWN_SPIDER_STATE_IDLE;
	static const int SMALL_BROWN_SPIDER_STATE_MOVE;
	static const int SMALL_BROWN_SPIDER_STATE_SLASH;
	static const int SMALL_BROWN_SPIDER_STATE_DASH;
	static const int SMALL_BROWN_SPIDER_STATE_TELEGRAPH_DASH;
	static const int SMALL_BROWN_SPIDER_STATE_DEAD;

	//phases
	static const int SMALL_BROWN_SPIDER_PHASE_NORMAL;
	static const int SMALL_BROWN_SPIDER_PHASE_DAMAGED;
	static const int SMALL_BROWN_SPIDER_PHASE_FRANTIC;

	// ai
	static const int SMALL_BROWN_SPIDER_AI_NORMAL;
	static const int SMALL_BROWN_SPIDER_AI_CHASE;

	//when 0, do new acton
	float thinkTimer = 0;

	//who should i chase
	LivingEntity* target = NULL;

	int aiState = SMALL_BROWN_SPIDER_AI_NORMAL;

	int phase;

	SmallBrownSpider(AnimationSet* animSet);
	void update();
	void think();
	void slash();
	void dash();
	void telegraphDash();
	void updateDirection();
	void die();
	void findNearestTarget();
	void changeAnimation(int newState, bool resetFrameToBeginning, string animName = "");
	void updateAnimation();
	void updateDamages();
	void pursueTarget(LivingEntity* entity);

private:
	int thinkTimerModifier;
	int actionModifier;
};

#endif // !SMALLBROWNSPIDER
