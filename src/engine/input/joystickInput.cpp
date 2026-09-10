#include "joystickInput.h"

#include <cmath>

namespace {
const double PI = 3.14159265358979323846;
}

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


std::vector<InputCommand> JoystickInput::update(SDL_Event* e) {
	std::vector<InputCommand> commands;

	if (e->type == SDL_JOYBUTTONDOWN) {
		if (e->jbutton.button == JOY_B) {
			commands.push_back({ InputCommandType::Dash, InputSource::Joystick });
		}

		if (e->jbutton.button == JOY_A) {
			commands.push_back({ InputCommandType::Action, InputSource::Joystick });
		}

		if (e->jbutton.button == JOY_Y) {
			commands.push_back({ InputCommandType::UseItem, InputSource::Joystick });
		}

		if (e->jbutton.button == JOY_X) {
			commands.push_back({ InputCommandType::Attack, InputSource::Joystick });
		}

		if (e->jbutton.button == JOY_RB) {
			commands.push_back({ InputCommandType::NextQuickItem, InputSource::Joystick });
		}
	}

	if (e->type == SDL_JOYAXISMOTION) {
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
			double joystickAngle = atan2((double)yDir, (double)xDir) * (180.0 / PI);

			//Correct angle
			if (xDir == 0 && yDir == 0) {
				commands.push_back({ InputCommandType::Stop, InputSource::Joystick });
			}
			else {
				commands.push_back({ InputCommandType::Move, InputSource::Joystick, static_cast<float>(joystickAngle) });
			}
		}
	}
	else {
		if (xDir != 0 || yDir != 0) {
			double joystickAngle = atan2((double)yDir, (double)xDir) * (180.0 / PI);
			commands.push_back({ InputCommandType::Move, InputSource::Joystick, static_cast<float>(joystickAngle) });
		}
		else {
			commands.push_back({ InputCommandType::Stop, InputSource::Joystick });
		}
	}

	return commands;
}

std::vector<InputCommand> JoystickInput::checkAxis() {
	std::vector<InputCommand> commands;
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

		if (xDir != 0 || yDir != 0) {
			double joystickAngle = atan2((double)yDir, (double)xDir) * (180.0 / PI);
			commands.push_back({ InputCommandType::Move, InputSource::Joystick, static_cast<float>(joystickAngle) });
		}
		else {
			commands.push_back({ InputCommandType::Stop, InputSource::Joystick });
		}

		return commands;
}
