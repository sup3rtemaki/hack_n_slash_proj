#include "joystickInput.h"

JoystickInput::JoystickInput() {
	gGameController = NULL;

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

		if (e->jbutton.button == JOY_A) {
			hero->takeAction();
		}

		if (e->jbutton.button == JOY_Y) {
			hero->useSelectedItemQuickAccess();
		}

		if (e->jbutton.button == JOY_X) {
			hero->attack();
		}

		if (e->jbutton.button == JOY_RB) {
			hero->quickAccessInventoryIndex++;
			if (hero->quickAccessInventory[hero->quickAccessInventoryIndex] == -1) {
				hero->quickAccessInventoryIndex = 0;
			}
		}
	}

	if (e->type == SDL_JOYAXISMOTION) {
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
			if (xDir == 0 && yDir == 0) {
				hero->moving = false;
			}
			else {
				hero->move(joystickAngle);
			}
		}
	}
	else {
		if (xDir != 0 || yDir != 0) {
			double joystickAngle = atan2((double)yDir, (double)xDir) * (180.0 / M_PI);
			hero->move(joystickAngle);
		}
		else if (hero->isMovingMethod != 1) {
			hero->moving = false;
			hero->isMovingMethod = 0;
		}
	}
}

void JoystickInput::checkAxis() {
	// pre-check both analog axis before checking the axis motion event
	// this causes some code duplication, but solves resuming the movement after pressing a button
	// (the axis dont generate a new event if they are interrupted e kept at the same position)
	// pre-check X axis
	if (SDL_JoystickGetAxis(gGameController, X_AXIS_MOTION) > JOYSTICK_DEAD_ZONE) {
		xDir = 1;
	}
	else if (SDL_JoystickGetAxis(gGameController, X_AXIS_MOTION) < -JOYSTICK_DEAD_ZONE) {
		xDir = -1;
	}
	else {
		xDir = 0;
	}

	// pre-check Y axis
	if (SDL_JoystickGetAxis(gGameController, Y_AXIS_MOTION) > JOYSTICK_DEAD_ZONE) {
		yDir = 1;
	}
	else if (SDL_JoystickGetAxis(gGameController, Y_AXIS_MOTION) < -JOYSTICK_DEAD_ZONE) {
		yDir = -1;
	}
	else {
		yDir = 0;
	}
}
