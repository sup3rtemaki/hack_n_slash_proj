#ifndef KEY
#define KEY

#include "item.h"
class Key : public Item {
public:
	Key(bool isOnGround, int quant, SDL_Renderer* renderer);

	void update();
	void changeAnimation(int newState = 0, bool resetFrameToBeginning = true, string animName = "");
	void applyEffect(LivingEntity* hero);
};

#endif // !KEY
