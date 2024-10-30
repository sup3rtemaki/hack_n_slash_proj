#include "globals.h"

const float Globals::PI = 3.14159f;

//useful as a dev
bool Globals::debugging = false;

bool Globals::pause = false;

//sdl
int Globals::ScreenWidth = 640, Globals::ScreenHeight = 360, Globals::ScreenScale = 3;
SDL_Renderer* Globals::renderer = NULL;

string Globals::clipOffDataHeader(string data) {
	int pos = data.find(":", 0);// returns where we find the : in string
	if (pos != -1) {
		data = data.substr(pos + 1, data.length() - pos + 2);
	}
	return data;
}

SDL_Rect Globals::camera;