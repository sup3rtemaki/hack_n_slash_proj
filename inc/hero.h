#ifndef HERO
#define HERO

#include "globals.h"
#include "livingEntity.h"
#include "soundManager.h"
#include "itemsHub.h"
#include "map.h"
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

	static const string HERO_CONSUMING_ANIM_UP;
	static const string HERO_CONSUMING_ANIM_DOWN;
	static const string HERO_CONSUMING_ANIM_LEFT;
	static const string HERO_CONSUMING_ANIM_RIGHT;

	static const string HERO_ANIM_DIE;

	static const int HERO_STATE_IDLE;
	static const int HERO_STATE_MOVE;
	static const int HERO_STATE_SLASH;
	static const int HERO_STATE_DASH;
	static const int HERO_STATE_DEAD;
	static const int HERO_STATE_CONSUMING_ITEM;

	int isMovingMethod = 0; // TODO: Paliativo, mudar depois 0 = not moving, 1 = keyboard, 2 = joystick
	int inventoryIndex = 0;
	int honeydewQty;
	int qtyItemsPicked;

	map<int, Item*> inventory;
	vector<int> quickAccessInventory = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	unordered_map<int, Item*> passiveSlots;
	list<Item*> nearItems;
	string addedItemName;

	Map* currentMap;

	Hero(AnimationSet* animSet);

	void update();
	void slash();
	void dash();
	void die();
	void revive();
	void changeAnimation(int newState, bool resetFrameToBeginning, string animName = "");
	void updateAnimation();
	void updateDamages();
	//void checkNearItem(Item* item);
	void addItemToInventory(Item* item);
	void addItemToQuickAccess(int itemId);
	void useSelectedItem(int invIndex);
	void useSelectedItemQuickAccess();
	void pickNearItemFromGround();

	void statusTimerTick();
	void healTimerTick();

private:
	void findNearestItem();
	Item* currentNearItem;
};

#endif