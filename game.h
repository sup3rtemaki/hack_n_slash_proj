#ifndef GAME
#define GAME

#include "globals.h"
#include "hero.h"
#include "wall.h"
#include "glob.h"
#include "keyboardInput.h"
#include "drawing_functions.h"
#include "soundManager.h"

class Game {
public:
	Mix_Music* song;

	AnimationSet* heroAnimSet;
	AnimationSet* globAnimSet;
	AnimationSet* wallAnimSet;

	SDL_Texture* backGroundImage;
	SDL_Texture* splashImage;
	SDL_Texture* overlayImage;

	SDL_Texture* scoreTexture = NULL; //draw string to screen

	Hero* hero;
	KeyboardInput heroInput;

	list<Entity*> enemies;
	list<Entity*> walls;

	bool splashShowing;
	float overlayTimer;

	Game();
	~Game();

	void update();
	void draw();
};

#endif // !GAME

