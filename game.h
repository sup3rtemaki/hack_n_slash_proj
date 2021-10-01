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
	SDL_Texture* backGroundImageN;
	SDL_Texture* backGroundImageNW;
	SDL_Texture* backGroundImageW;
	SDL_Texture* backGroundImageSW;
	SDL_Texture* backGroundImageS;
	SDL_Texture* backGroundImageSE;
	SDL_Texture* backGroundImageE;
	SDL_Texture* backGroundImageNE;
	SDL_Texture* splashImage;
	SDL_Texture* overlayImage;

	SDL_Texture* scoreTexture = NULL; //draw string to screen

	int currentMapId;
	Map currentMap;
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

	CameraController camController;

	Game();
	~Game();

	void update();
	void draw();
};

#endif // !GAME

