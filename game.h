#ifndef GAME
#define GAME

#include "globals.h"
#include "hero.h"
#include "wall.h"
#include "glob.h"
#include "grob.h"
#include "keyboardInput.h"
#include "drawing_functions.h"
#include "soundManager.h"
#include "cameraController.h"
#include "roundKing.h"
#include "hpBar.h"
#include "map.h"

class Game {
public:
	Mix_Music* song;

	AnimationSet* heroAnimSet;
	AnimationSet* globAnimSet;
	AnimationSet* grobAnimSet;
	AnimationSet* wallAnimSet;
	AnimationSet* roundKingAnimSet;
	AnimationSet* bulletAnimSet;

	SDL_Texture* backGroundImage;
	SDL_Texture* backGroundImage2;
	SDL_Texture* splashImage;
	SDL_Texture* overlayImage;

	SDL_Texture* scoreTexture = NULL; //draw string to screen

	list<Map> mapList;

	Hero* hero;
	KeyboardInput heroInput;

	list<Entity*> enemies;
	list<Entity*> walls;

	int enemyWavesTillBoss = 3;
	bool buildBossNext = false;
	bool bossActive = false;
	
	HPBar heroHpBar;
	HPBar bossHpBar;

	bool splashShowing;
	float overlayTimer;

	int map;

	CameraController camController;

	Game();
	~Game();

	void update();
	void draw();
};

#endif // !GAME

