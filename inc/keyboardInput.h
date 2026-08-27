#ifndef KEYBOARDINPUT
#define KEYBOARDINPUT

#include "SDL.h"
#include "inputCommand.h"
#include <vector>

class KeyboardInput {
public:
	SDL_Scancode UP, DOWN, LEFT, RIGHT;
	SDL_Scancode ATTACK, DASH;
	SDL_Scancode USE_ITEM;
	SDL_Scancode ACTION;

	KeyboardInput();
	std::vector<InputCommand> update(SDL_Event* e);
};

#endif // !KEYBOARDINPUT

