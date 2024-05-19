#ifndef KEY
#define KEY

#include "item.h"
class Key : public Item {
public:
	Key(AnimationSet* animSet, bool isOnGround, int quant);

	void update();
	void changeAnimation(int newState = 0, bool resetFrameToBeginning = true, string animName = "");
	void applyEffect(LivingEntity* hero);
};

#endif // !KEY
