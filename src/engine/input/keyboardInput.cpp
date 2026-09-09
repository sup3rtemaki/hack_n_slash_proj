#include "keyboardInput.h"

KeyboardInput::KeyboardInput() {
	//hardcoded
	UP = SDL_SCANCODE_UP;
	DOWN = SDL_SCANCODE_DOWN;
	LEFT = SDL_SCANCODE_LEFT;
	RIGHT = SDL_SCANCODE_RIGHT;
	ATTACK = SDL_SCANCODE_Z;
	DASH = SDL_SCANCODE_X;
	USE_ITEM = SDL_SCANCODE_A;
	ACTION = SDL_SCANCODE_S;
}

std::vector<InputCommand> KeyboardInput::update(SDL_Event* e) {
	std::vector<InputCommand> commands;

	//button presses
	if (e->type == SDL_KEYDOWN) {
		if (e->key.keysym.scancode == DASH) {
			commands.push_back({ InputCommandType::Dash, InputSource::Keyboard });
		}
		if (e->key.keysym.scancode == ATTACK) {
			commands.push_back({ InputCommandType::Attack, InputSource::Keyboard });
		}
		if (e->key.keysym.scancode == USE_ITEM) {
			commands.push_back({ InputCommandType::UseItem, InputSource::Keyboard });
		}
		if (e->key.keysym.scancode == ACTION) {
			commands.push_back({ InputCommandType::Action, InputSource::Keyboard });
		}

		if (e->key.keysym.scancode == SDL_SCANCODE_F) {
			commands.push_back({ InputCommandType::NextQuickItem, InputSource::Keyboard });
		}
	}

	//button holds
	const Uint8* keystates = SDL_GetKeyboardState(NULL); //check for keys still being held
	if (!keystates[UP] && !keystates[DOWN] && !keystates[RIGHT] && !keystates[LEFT]) {
		commands.push_back({ InputCommandType::Stop, InputSource::Keyboard });
	}
	else {
		commands.push_back({ InputCommandType::Move, InputSource::Keyboard, 0 });
		//up
		if (keystates[UP]) {
			if (keystates[RIGHT]) {
				commands.back().angle = 270 + 45;
			}
			else if (keystates[LEFT]) {
				commands.back().angle = 270 - 45;
			}
			else {
				commands.back().angle = 270;
			}
		}
		//down
		if (keystates[DOWN]) {
			if (keystates[RIGHT]) {
				commands.back().angle = 90 - 45;
			}
			else if (keystates[LEFT]) {
				commands.back().angle = 90 + 45;
			}
			else {
				commands.back().angle = 90;
			}
		}
		//left
		if (!keystates[UP] && !keystates[DOWN] && !keystates[RIGHT] && keystates[LEFT]) {
			commands.back().angle = 180;
		}
		//right
		if (!keystates[UP] && !keystates[DOWN] && keystates[RIGHT] && !keystates[LEFT]) {
			commands.back().angle = 0;
		}
	}

	return commands;
}