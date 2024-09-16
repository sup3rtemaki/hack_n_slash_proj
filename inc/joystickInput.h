#ifndef JOYSTICKINPUT
#define JOYSTICKINPUT

#include "hero.h"

class JoystickInput {
public:
	class JoystickInput();
	void update(SDL_Event* e);
	void checkAxis();

	Hero* hero;

private:
	const int JOY_Y = 0;
	const int JOY_B = 1;
	const int JOY_A = 2;
	const int JOY_X = 3;
	const int JOY_LB = 4;
	const int JOY_RB = 5;
	const int JOY_LT = 6;
	const int JOY_RT = 7;
	const int X_AXIS_MOTION = 0;
	const int Y_AXIS_MOTION = 1;
	const int JOYSTICK_DEAD_ZONE = 8000;

	int xDir = 0;
	int yDir = 0;

	SDL_Joystick* gGameController;
};

#endif // !JOYSTICKINPUT

