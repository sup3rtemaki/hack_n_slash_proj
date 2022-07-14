#ifndef HONEYDEWPOTION
#define HONEYDEWPOTION

#include "item.h"

class HoneydewPotion : public Item {
public:
	HoneydewPotion(AnimationSet* animSet, bool isOnGround, int quant);
	~HoneydewPotion();

	void update();
	void changeAnimation(int newState = 0, bool resetFrameToBeginning = true, string animName = "");
	void applyEffect(LivingEntity* heroEntity);
};

#endif // !HONEYDEWPOTION

