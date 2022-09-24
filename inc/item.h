#ifndef ITEM
#define ITEM

#include "livingEntity.h"

class Item : public Entity {
public:
	static const int HONEYDEW_POTION_ID = 0;
	static const string ITEM_CONSUMABLE;
	static const string ITEM_PASSIVE;
	static const string ITEM_KEY;

	int id;
	int quantity;
	string name;
	string itemType;
	string description;
	bool isOnGround;
	bool isNearHero;

	Item(AnimationSet* animSet = nullptr);
	~Item();
	void update();
	void changeAnimation(int newState = 0, bool resetFrameToBeginning = true, string animName = "");
	virtual void applyEffect(LivingEntity* heroEntity) { ; };
};

#endif // !ITEM

