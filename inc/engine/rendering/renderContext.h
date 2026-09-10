#ifndef RENDERCONTEXT
#define RENDERCONTEXT

#include "SDL.h"

struct RenderContext {
	SDL_Renderer* renderer = nullptr;
	SDL_Rect camera{ 0, 0, 0, 0 };
	bool debugging = false;
};

#endif
