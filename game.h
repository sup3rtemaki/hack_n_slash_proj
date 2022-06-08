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
#include<thread>

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
	SDL_Texture* fadeImage;
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

	bool isFading = false;
	bool fadeIn, fadeOut;
	int alpha = 0;
	float alphaCalc;

	int currentMapId, lastMapId;
	int mapToDrawCount;
	Map currentMap;
	list<Map> mapList;
	enum class NextMap {
		NONE,
		LEFT,
		TOP,
		RIGHT,
		BOTTOM
	};
	NextMap nextMap;

	Hero* hero;
	KeyboardInput heroInput;

	list<Entity*> currentMapEnemies;
	list<Entity*> walls;
	list<int> deadEnemiesIds;

	int enemyWavesTillBoss = 3;
	bool buildBossNext = false;
	bool bossActive = false;
	bool mustSpawnEnemies = false;
	
	HPBar heroHpBar;
	HPBar bossHpBar;

	bool splashShowing;
	float overlayTimer;

	CameraController camController;

	Game();
	~Game();

	void update();
	void draw();
	void updateMaps();
};

#endif // !GAME

