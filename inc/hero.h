#ifndef HERO
#define HERO

#include "globals.h"
#include "livingEntity.h"
#include "soundManager.h"
#include "item/itemsHub.h"
#include "map.h"
#include <map>
#include <unordered_map>
#include <vector>
#include <deque>

enum class HeroActions {
	PICK_NEAR_ITEM,
	OPEN_DOOR,
	NONE
};

enum class HERO_STATE {
	IDLE,
	MOVE,
	ATTACK_1,
	ATTACK_2,
	ATTACK_3,
	DASH,
	ROLL,
	DEAD,
	CONSUMING_ITEM,
	SHOOTING,
	RESTING
};

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

	static const string HERO_ANIM_REST_UP;
	static const string HERO_ANIM_REST_DOWN;
	static const string HERO_ANIM_REST_LEFT;
	static const string HERO_ANIM_REST_RIGHT;

	static const string HERO_SLASH_ANIM_UP;
	static const string HERO_SLASH_ANIM_DOWN;
	static const string HERO_SLASH_ANIM_LEFT;
	static const string HERO_SLASH_ANIM_RIGHT;

	static const string HERO_SLASH2_ANIM_UP;
	static const string HERO_SLASH2_ANIM_DOWN;
	static const string HERO_SLASH2_ANIM_LEFT;
	static const string HERO_SLASH2_ANIM_RIGHT;

	static const string HERO_SLASH3_ANIM_UP;
	static const string HERO_SLASH3_ANIM_DOWN;
	static const string HERO_SLASH3_ANIM_LEFT;
	static const string HERO_SLASH3_ANIM_RIGHT;

	static const string HERO_DASH_ANIM_UP;
	static const string HERO_DASH_ANIM_DOWN;
	static const string HERO_DASH_ANIM_LEFT;
	static const string HERO_DASH_ANIM_RIGHT;

	static const string HERO_ROLL_ANIM_UP;
	static const string HERO_ROLL_ANIM_DOWN;
	static const string HERO_ROLL_ANIM_LEFT;
	static const string HERO_ROLL_ANIM_RIGHT;

	static const string HERO_CONSUMING_ANIM_UP;
	static const string HERO_CONSUMING_ANIM_DOWN;
	static const string HERO_CONSUMING_ANIM_LEFT;
	static const string HERO_CONSUMING_ANIM_RIGHT;

	static const string HERO_SHOOT_ANIM_UP;
	static const string HERO_SHOOT_ANIM_DOWN;
	static const string HERO_SHOOT_ANIM_LEFT;
	static const string HERO_SHOOT_ANIM_RIGHT;

	static const string HERO_ANIM_DIE;

	static const int PHEROMONE_TRAIL_MAX_SIZE;

	bool isCheckpointActivatedFlag;
	bool isRested;
	bool isAttacking;
	bool mustSaveGame;
	bool mustMoveAfterAction;
	bool mustUpdateKeyJoyInput;

	SDL_Point lastCheckpointPos;
	string lastCheckpointMapFile;

	int isMovingMethod = 0; // TODO: Paliativo, mudar depois 0 = not moving, 1 = keyboard, 2 = joystick
	int inventoryIndex = 0;
	int quickAccessInventoryIndex = 0;
	int honeydewQty;
	int qtyItemsPicked;
	int attackBufferIndex;
	int prevAttackState;
	int itemIdToUseAfterClosingMenu;

	float attackTimer;

	class Door* nearestDoor;
	class Checkpoint* nearestCheckpoint;
	class Bloodstain* nearestBloodstain;
	map<int, Item*> inventory;
	vector<int> quickAccessInventory = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	vector<int> comboSequence;
	deque<int> attackBuffer;
	unordered_map<int, Item*> passiveSlots;
	list<Item*> nearItems;
	string addedItemName;

	Map* currentMap;
	class ActionMessageUi* actionMessageUi;

	Hero(AnimationSet* animSet);

	void update();
	void move(float angle);
	void attack();
	void dash();
	void roll();
	void die();
	void revive();
	void rest();
	void changeAnimation(int newState, bool resetFrameToBeginning, string animName = "");
	void updateAnimation();
	void updateDamages();
	void takeAction();
	void addEssence(int essenceQty);
	//void checkNearItem(Item* item);
	void addItemToInventory(Item* item);
	void addItemToQuickAccess(int itemId, int position = -1);
	void useSelectedItem(int invIndex);
	void useSelectedItemQuickAccess();
	void pickNearItemFromGround();
	void openDoor();

	void statusTimerTick();
	void healTimerTick();
	void attackTimerTick();

private:
	int newEssenceQty;
	int prevEssence;

	float pheromoneTimer;
	float pheromoneMaxTime = 0.1f;

	void findNearestItem();
	void updateEssence();
	void updateAttackSequence();
	void updatePheromoneTrail();
	Item* currentNearItem;
};

#endif