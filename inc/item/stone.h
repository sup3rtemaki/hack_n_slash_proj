#pragma once
#include "item.h"
class Stone : public Item {
public:
	Stone(AnimationSet* animSet, bool isOnGround, int quant);

	void update();
	void changeAnimation(int newState = 0, bool resetFrameToBeginning = true, string animName = "");
	void applyEffect(LivingEntity* heroEntity);
};

