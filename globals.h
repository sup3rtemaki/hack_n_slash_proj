#ifndef GLOBALS
#define GLOBALS

#include <string>
#include <iostream>
#include <SDL.h>
#include "randomNumber.h"

using namespace std;

class Globals {
public:
	//math
	static const float PI;

	//useful as a dev
	static bool debugging;

	//sdl
	static int ScreenWidth, ScreenHeight, ScreenScale;
	static SDL_Renderer* renderer;

	//clips off header
	static string clipOffDataHeader(string data);

	//camera
	static SDL_Rect camera;
};

#endif // !GLOBALS

