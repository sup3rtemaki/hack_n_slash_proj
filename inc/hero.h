#ifndef HERO
#define HERO

#include "globals.h"
#include "livingEntity.h"
#include "soundManager.h"
#include "item.h"
#include <map>
#include <unordered_map>
#include <vector>

class Hero : public LivingEntity {
public:
	static const string HERO_ANIM_UP;
	static const string HERO_ANIM_DOWN;
	static const string HERO_ANIM_LEFT;
	static const string HERO_ANIM_RIGHT;

	static const string HERO_ANIM_IDLE_UP;
	static const string HERO_ANIM_IDLE_DOWN;
	static const string HERO_ANIM_IDLE_LEFT;
	static const string HERO_ANIM_IDLE_RIGHT;

	static const string HERO_SLASH_ANIM_UP;
	static const string HERO_SLASH_ANIM_DOWN;
	static const string HERO_SLASH_ANIM_LEFT;
	static const string HERO_SLASH_ANIM_RIGHT;

	static const string HERO_DASH_ANIM_UP;
	static const string HERO_DASH_ANIM_DOWN;
	static const string HERO_DASH_ANIM_LEFT;
	static const string HERO_DASH_ANIM_RIGHT;

	static const string HERO_ANIM_DIE;

	static const int HERO_STATE_IDLE;
	static const int HERO_STATE_MOVE;
	static const int HERO_STATE_SLASH;
	static const int HERO_STATE_DASH;
	static const int HERO_STATE_DEAD;

	int inventoryIndex = 0;
	int honeydewQty;

	bool nearItem;

	map<int, Item*> inventory;
	vector<int> quickAccessInventory = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	unordered_map<int, Item*> passiveSlots;

	Hero(AnimationSet* animSet);
	~Hero();

	void update();
	void slash();
	void dash();
	void die();
	void revive();
	void changeAnimation(int newState, bool resetFrameToBeginning, string animName = "");
	void updateAnimation();
	void updateDamages();
	bool isNearItem(Item* item);
	void addItemToInventory(Item* item);
	void addItemToQuickAccess(int itemId);
	void useSelectedItem(int invIndex);
	void useSelectedItemQuickAccess();
};

#endif