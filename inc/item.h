#ifndef ITEM
#define ITEM

#include "livingEntity.h"

class Item : public Entity {
public:
	static const int HONEYDEW_POTION_ID;

	int id;
	int quantity;
	string name;
	string description;
	bool isOnGround;


	Item(AnimationSet* animSet = nullptr);
	~Item();
	void update();
	void changeAnimation(int newState = 0, bool resetFrameToBeginning = true);
	void applyEffect(LivingEntity* heroEntity);
};

#endif // !ITEM

