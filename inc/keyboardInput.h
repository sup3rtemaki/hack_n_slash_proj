#ifndef KEYBOARDINPUT
#define KEYBOARDINPUT

#include "hero.h"

class KeyboardInput {
public:
	Hero* hero;
	SDL_Scancode UP, DOWN, LEFT, RIGHT;
	SDL_Scancode ATTACK, DASH;
	SDL_Scancode USE_ITEM;
	SDL_Scancode ACTION;

	KeyboardInput();
	void update(SDL_Event* e);
};

#endif // !KEYBOARDINPUT

