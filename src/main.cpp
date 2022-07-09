#include "cleanup.h"
#include "res_path.h"
#include "drawing_functions.h"
#include "SDL_mixer.h"
#include "globals.h"
#include "game.h"
#include <cstdlib> //srand, rand
#include <ctime> //time

int main(int argc, char** argv) {

	//Random Seed
	srand(time(0));

	//Setup SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		cout << "error Initializing SDL" << endl;
		return 1;
	}

	//Setup Window
	SDL_Window* window = SDL_CreateWindow("Ant Hero",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		Globals::ScreenWidth * Globals::ScreenScale,
		Globals::ScreenHeight * Globals::ScreenScale,
		SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE); //SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN
	if (window == nullptr) {
		SDL_Quit();
		cout << "window error" << endl;
		return 1;
	}

	//Setup Renderer
	Globals::renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (Globals::renderer == nullptr) {
		cleanup(window);
		SDL_Quit();
		cout << "renderer error" << endl;
		return 1;
	}

	SDL_RenderSetLogicalSize(Globals::renderer, 
		Globals::ScreenWidth,
		Globals::ScreenHeight);

	//Initialize sdl_image
	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		SDL_Quit();
		cout << "sdl image didnt initialize" << endl;
		return 1;
	}

	//Initialize ttf
	if (TTF_Init() != 0) {
		SDL_Quit();
		cout << "ttf didnt initialize" << endl;
		return 1;
	}

	//Initialize mixer
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
		SDL_Quit();
		cout << "mixer didnt initialize" << endl;
		return 1;
	}

	//Load a texture to draw
	/*string resPath = getResourcePath();
	SDL_Texture* texture = loadTexture(resPath + "map.png", Globals::renderer);

	//run game for 5000 ticks
	while (SDL_GetTicks() < 5000){
		//clear screen
		SDL_RenderClear(Globals::renderer);
		//draw
		renderTexture(texture, Globals::renderer, 0, 0);
		//show image
		SDL_RenderPresent(Globals::renderer);
	}*/


	Game game;
	game.update();

	cleanup(Globals::renderer);
	cleanup(window);
	//cleanup(texture);

	SDL_Quit();
	return 0;
}