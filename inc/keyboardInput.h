#ifndef KEYBOARDINPUT
#define KEYBOARDINPUT

#include "hero.h"

class KeyboardInput {
public:
	Hero* hero;
	SDL_Scancode UP, DOWN, LEFT, RIGHT;
	SDL_Scancode SLASH, DASH;

	KeyboardInput();
	void update(SDL_Event* e);
};

#endif // !KEYBOARDINPUT

