#ifndef ITEM
#define ITEM

#include "livingEntity.h"

class Item : public Entity {
public:
	static const int HONEYDEW_POTION_ID = 0;
	static const int GREEN_BERRY_ID = 1;
	static const int STONE_ID = 2;
	static const int COMMON_KEY_ID = 3;
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
	SDL_Texture* image;
	AnimationSet* projectileAnimSet; // Apenas para itens projéteis

	Item(SDL_Texture* image = nullptr, AnimationSet* animSet = nullptr);
	~Item();
	void update();
	void changeAnimation(int newState = 0, bool resetFrameToBeginning = true, string animName = "");
	virtual void applyEffect(LivingEntity* heroEntity) { ; };
};

#endif // !ITEM

