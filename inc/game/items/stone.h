#pragma once
#include "item.h"
class Stone : public Item {
public:
	Stone(bool isOnGround, int quant, SDL_Renderer* renderer);

	void update();
	void changeAnimation(int newState = 0, bool resetFrameToBeginning = true, string animName = "");
	void applyEffect(LivingEntity* heroEntity);
	std::unique_ptr<AnimationSet> projectileLocalAnimSet;

private:
	SDL_Renderer* renderer;
};

