#include "keyboardInput.h"

KeyboardInput::KeyboardInput() {
	//hardcoded
	UP = SDL_SCANCODE_UP;
	DOWN = SDL_SCANCODE_DOWN;
	LEFT = SDL_SCANCODE_LEFT;
	RIGHT = SDL_SCANCODE_RIGHT;
	SLASH = SDL_SCANCODE_Z;
	DASH = SDL_SCANCODE_X;
}

void KeyboardInput::update(SDL_Event* e){
	//button presses
	if (e->type == SDL_KEYDOWN) {
		if (e->key.keysym.scancode == DASH) {
			hero->dash();
		}
		if (e->key.keysym.scancode == SLASH) {
			hero->slash();
		}
		if (e->key.keysym.scancode == SDL_SCANCODE_A) {
			hero->useSelectedItemQuickAccess();
		}
		if (e->key.keysym.scancode == SDL_SCANCODE_S) {
			if (hero->nearItem) {
				hero->pickNearItemFromGround();
			}
		}
	}

	//button holds
	const Uint8* keystates = SDL_GetKeyboardState(NULL); //check for keys still being held
	if (((hero->state != Hero::HERO_STATE_MOVE && hero->state != Hero::HERO_STATE_IDLE) ||
		(!keystates[UP] && !keystates[DOWN] && !keystates[RIGHT] && !keystates[LEFT])) &&
		hero->isMovingMethod != 2) {
		hero->moving = false;
		hero->isMovingMethod = 0;
	}
	else {
		hero->isMovingMethod = 1;
		//up
		if (keystates[UP]) {
			if (keystates[RIGHT]) {
				hero->move(270 + 45);
			}
			else if (keystates[LEFT]) {
				hero->move(270 - 45);
			}
			else {
				hero->move(270);
			}
		}
		//down
		if (keystates[DOWN]) {
			if (keystates[RIGHT]) {
				hero->move(90 - 45);
			}
			else if (keystates[LEFT]) {
				hero->move(90 + 45);
			}
			else {
				hero->move(90);
			}
		}
		//left
		if (!keystates[UP] && !keystates[DOWN] && !keystates[RIGHT] && keystates[LEFT]) {
			hero->move(180);
		}
		//right
		if (!keystates[UP] && !keystates[DOWN] && keystates[RIGHT] && !keystates[LEFT]) {
			hero->move(0);
		}
	}
}