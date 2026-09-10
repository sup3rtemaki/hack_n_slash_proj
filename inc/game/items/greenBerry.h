#ifndef GREENBERRY
#define GREENBERRY

#include "item.h"

class GreenBerry : public Item {
public:
	GreenBerry(bool isOnGround, int quant, SDL_Renderer* renderer);
	~GreenBerry();

	void update();
	void changeAnimation(int newState = 0, bool resetFrameToBeginning = true, string animName = "");
	void applyEffect(LivingEntity* heroEntity);
};

#endif // !GREENBERRY

