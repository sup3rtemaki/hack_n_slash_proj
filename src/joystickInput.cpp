#include "joystickInput.h"

JoystickInput::JoystickInput() {
	SDL_Joystick* gGameController = NULL;

	//Check for joysticks
	if (SDL_NumJoysticks() < 1)
	{
		printf("Warning: No joysticks connected!\n");
	}
	else
	{
		//Load joystick
		gGameController = SDL_JoystickOpen(0);
		if (gGameController == NULL)
		{
			printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
		}
	}
}

void JoystickInput::update(SDL_Event* e) {
	if (e->type == SDL_JOYBUTTONDOWN) {
		if (e->jbutton.button == JOY_B) {
			hero->dash();
		}

		if (e->jbutton.button == JOY_X) {
			hero->useSelectedItemQuickAccess();
		}

		if (e->jbutton.button == JOY_A) {
			hero->pickNearItemFromGround();
		}

		if (e->jbutton.button == JOY_RB) {
			hero->attack();
		}
	}

	// TODO: Debug, remover depois
	if (e->type == SDL_JOYBUTTONDOWN) {

		cout << "Botao:" << (int)e->jbutton.button << "\n";
	}

	if (e->type == SDL_JOYHATMOTION) {

		cout << "Hat:" << (int)e->jhat.hat << ", Valor: " << (int)e->jhat.value << "\n";
	}

	if (e->type == SDL_JOYBALLMOTION) {

		cout << "Ball:" << (int)e->jball.ball << ", Valor: " << (int)e->jball.xrel << "\n";
	}
	///////////////////////

	if (e->type == SDL_JOYAXISMOTION &&
		(hero->state == (int)HERO_STATE::MOVE || hero->state == (int)HERO_STATE::IDLE)) {
		hero->isMovingMethod = 2;
		////Motion on controller 0
		if (e->jaxis.which == 0) {
			//X axis motion
			if (e->jaxis.axis == X_AXIS_MOTION) {
				//Left of dead zone
				if (e->jaxis.value < -JOYSTICK_DEAD_ZONE) {
					xDir = -1;
				}
				//Right of dead zone
				else if (e->jaxis.value > JOYSTICK_DEAD_ZONE) {
					xDir = 1;
				}
				else {
					xDir = 0;
				}
			}
			//Y axis motion
			if (e->jaxis.axis == Y_AXIS_MOTION) {
				//Below of dead zone
				if (e->jaxis.value < -JOYSTICK_DEAD_ZONE) {
					yDir = -1;
				}
				//Above of dead zone
				else if (e->jaxis.value > JOYSTICK_DEAD_ZONE) {
					yDir = 1;
				}
				else {
					yDir = 0;
				}
			}

			//Calculate angle
			double joystickAngle = atan2((double)yDir, (double)xDir) * (180.0 / M_PI);

			//Correct angle
			if (xDir == 0 && yDir == 0)
			{
				joystickAngle = 0;
				hero->moving = false;
			}
			else {
				hero->move(joystickAngle);
			}
		}
	}
	else {
		if (hero->isMovingMethod != 1) {
			hero->moving = false;
			hero->isMovingMethod = 0;
		}
	}
}