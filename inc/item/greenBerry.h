#ifndef GREENBERRY
#define GREENBERRY

#include "item.h"

class GreenBerry : public Item {
public:
	GreenBerry(AnimationSet* animSet, bool isOnGround, int quant);
	~GreenBerry();

	void update();
	void changeAnimation(int newState = 0, bool resetFrameToBeginning = true, string animName = "");
	void applyEffect(LivingEntity* heroEntity);
};

#endif // !GREENBERRY

