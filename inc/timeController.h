#ifndef TIMECONTROLLER
#define TIMECONTROLLER

#include <iostream>
#include "SDL.h"

class TimeController {
public:
	static const int PLAY_STATE, PAUSE_STATE;
	int timeState;
	Uint32 lastUpdate;
	float dT;

	TimeController();
	void updateTime();
	void pause();
	void resume();
	void reset();

	static TimeController timeController;
};

#endif // !TIMECONTROLLER

