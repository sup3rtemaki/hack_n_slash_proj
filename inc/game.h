#ifndef GAME
#define GAME

#include "globals.h"
#include "hero.h"
#include "wall.h"
#include "glob.h"
#include "grob.h"
#include "termiteMiner.h"
#include "item/honeydewPotion.h"
#include "item/stone.h"
#include "keyboardInput.h"
#include "joystickInput.h"
#include "drawing_functions.h"
#include "soundManager.h"
#include "cameraController.h"
#include "roundKing.h"
#include "map.h"

#include "helpers/saveHandler.h"

#include "ui/actionMessageUi.h"
#include "ui/hpBar.h"
#include "ui/quickItemUi.h"
#include "ui/itemPickMessageUi.h"
#include "ui/essenceCounterUi.h"
#include "ui/menu/mainMenu.h"
#include "ui/menu/pauseMenu.h"

#include<thread>

enum class GameState {
	MainMenu,
	Loading,
	InGame,
	Paused,
	None
};

class Game {
public:
	Mix_Music* song;

	SDL_Texture* fadeImage;
	SDL_Texture* splashImage;
	SDL_Texture* overlayImage;
	SDL_Texture* scoreTexture = NULL; //draw string to screen

	SDL_Event event;

	/**
	* Cache de texturas
	* Lista que contem o nome da imagem e a textura SDL
	*/
	struct TextureDeleter {
		void operator()(SDL_Texture* tex) const {
			if (tex) {
				SDL_DestroyTexture(tex);
			}
		}
	};

	// Alias para facilitar uso
	using TexturePtr = std::unique_ptr<SDL_Texture, TextureDeleter>;

	std::map<string, TexturePtr> texturesCache;

	bool isFading = false;
	bool fadeIn, fadeOut;
	int alpha = 0;
	float alphaCalc;

	int currentMapId, lastMapId;
	int mapToDrawCount;
	Map* currentMap;
	list<Map> mapList;
	vector<int> openDoorsIds;
	vector<int> defeatedBossesIds;

	Hero* hero;
	KeyboardInput heroKeyboardInput;
	JoystickInput heroJoystickInput;

	LivingEntity* currentBoss;

	list<Entity*> currentMapEnemies;
	list<Entity*> walls;
	list<Entity*> fogWalls;
	list<Ui*> gui;
	list<int> deadEnemiesIds;
	list<Item*> itemsOnMap;

	int enemyWavesTillBoss = 3;
	bool buildBossNext = false;
	bool bossActive = false;
	bool mustSpawnEnemies = false;
	bool mustRemoveAllEnemies = false;
	bool quit = false;
	bool mustSetBloodstainLocation = true;

	
	HPBar* heroHpBar;
	HPBar* heroStBar;
	HPBar* bossHpBar;

	bool splashShowing;
	float overlayTimer;

	CameraController camController;

	int mapQty;

	SaveHandler saveHandler;

	QuickItemUi* quickItemUi;
	ItemPickMessageUi* itemPickMessageUi;
	ActionMessageUi* actionMessageUi;
	EssenceCounterUi* essenceCounterUi;
	MainMenu* mainMenu;
	PauseMenu* pauseMenu;

	GameState gameState;

	Game();
	~Game();

	void update();
	void draw();

private:
	std::unique_ptr<AnimationSet> heroAnimSet;
	std::unique_ptr<AnimationSet> globAnimSet;
	std::unique_ptr<AnimationSet> grobAnimSet;
	std::unique_ptr<AnimationSet> termiteMinerAnimSet;
	std::unique_ptr<AnimationSet> wallAnimSet;
	std::unique_ptr<AnimationSet> roundKingAnimSet;
	std::unique_ptr<AnimationSet> smallBrownSpiderAnimSet;
	std::unique_ptr<AnimationSet> bulletAnimSet;
	std::unique_ptr<AnimationSet> hDewPotionAnimSet;
	std::unique_ptr<AnimationSet> stoneProjectileAnimSet;
	std::unique_ptr<AnimationSet> doubleDoorsAnimSet;
	std::unique_ptr<AnimationSet> checkpointAnimSet;
	std::unique_ptr<AnimationSet> bloodstainAnimSet;
	SDL_Texture* gameCanvas = nullptr;
	int frameCount = 0;
	string resPath;

	class Bloodstain* bloodstain;

	bool isBossMap();
	bool isLivingEntityDead(Entity* entity);

	void runMainMenu();
	void runMainGame();
	void runPausedGameMenu();
	void updateMaps();
	void loadTiledMap(const string& mapFile);
	void renderTiles();
	void checkAndHandleEnemyLoot(Entity* entity);
	void checkAndHandleNearItem(Entity* entity);
	void checkAndHandleNearDoor(Entity* entity);
	void checkAndHandleNearCheckpoint(Entity* entity);
	void checkAndHandleNearBloodstain(Entity* entity);
	void handleMapChange(bool isHeroRespawn = false);
	void buildDoors();
	void buildWalls();
	void buildWaypoints();
	void spawnEnemies();
	void spawnBoss();
	void spawnItem(int itemId, int quant, int xPos, int yPos);
	void spawnCheckpoints();
	void checkBossDeath();
	void saveCheckpointActivatedState(int checkpointId);
	void loadAnimationSets();
	void spawnItemsFromCurrentMap();
	void inactivateCurrentMapItems();
	void removeAllEnemiesInMap();
	map<int, std::unique_ptr<Item>> loadInventoryItems(std::vector<std::pair<int, int>> items);
	void saveGame(bool isCheckpointSave = false);
	void loadGame();
	GameState getGameState();
};

#endif // !GAME

