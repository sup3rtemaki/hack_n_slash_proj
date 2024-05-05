#ifndef GAME
#define GAME

#include "globals.h"
#include "hero.h"
#include "wall.h"
#include "glob.h"
#include "grob.h"
#include "termiteMiner.h"
#include "honeydewPotion.h"
#include "stone.h"
#include "keyboardInput.h"
#include "joystickInput.h"
#include "drawing_functions.h"
#include "soundManager.h"
#include "cameraController.h"
#include "roundKing.h"
#include "hpBar.h"
#include "map.h"

#include "helpers/saveHandler.h"

#include "ui/quickItemUi.h"
#include "ui/itemPickMessageUi.h"

#include<thread>

class Game {
public:
	Mix_Music* song;

	AnimationSet* heroAnimSet;
	AnimationSet* globAnimSet;
	AnimationSet* grobAnimSet;
	AnimationSet* termiteMinerAnimSet;
	AnimationSet* wallAnimSet;
	AnimationSet* roundKingAnimSet;
	AnimationSet* bulletAnimSet;
	AnimationSet* hDewPotionAnimSet;
	AnimationSet* stoneProjectileAnimSet;
	AnimationSet* doubleDoorsAnimSet;

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

	/**
	* Cache de texturas
	* Lista que contem o nome da imagem e a textura SDL
	*/
	std::map<string, SDL_Texture*> texturesCache;

	bool isFading = false;
	bool fadeIn, fadeOut;
	int alpha = 0;
	float alphaCalc;

	int currentMapId, lastMapId;
	int mapToDrawCount;
	Map* currentMap;
	list<Map> mapList;

	Hero* hero;
	KeyboardInput heroKeyboardInput;
	JoystickInput heroJoystickInput;

	list<Entity*> currentMapEnemies;
	list<Entity*> walls;
	list<int> deadEnemiesIds;
	list<Item*> itemsOnMap;

	int enemyWavesTillBoss = 3;
	bool buildBossNext = false;
	bool bossActive = false;
	bool mustSpawnEnemies = false;
	
	HPBar heroHpBar;
	HPBar bossHpBar;

	bool splashShowing;
	float overlayTimer;

	CameraController camController;

	int mapQty;

	SaveHandler saveHandler;

	QuickItemUi* quickItemUi;
	ItemPickMessageUi* itemPickMessageUi;

	Game();
	~Game();

	void update();
	void draw();

private:
	void updateMaps();
	void loadTiledMap(const string& mapFile);
	void renderTiles();
	void buildDoors();
	void buildWalls();
	void buildWaypoints();
	void spawnEnemies();
	void spawnItem(int itemId, int quant, int xPos, int yPos);
	void loadAnimationSets();
	void spawnItemsFromCurrentMap();
	void inactivateCurrentMapItems();
	map<int, Item*> loadInventoryItems(std::vector<std::pair<int, int>> items);
	void saveGame();
	void loadGame();
};

#endif // !GAME

