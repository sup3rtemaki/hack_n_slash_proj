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
#include "timeController.h"
#include "renderContext.h"
#include "inputCommand.h"
#include "tiledMapLoader.h"
#include "npcs/npcFactory.h"

#include <map>
#include <tuple>

namespace tson {
class Map;
class Tile;
}

#include "helpers/saveHandler.h"
#include "helpers/gameSaveManager.h"

#include "systems/interactionSystem.h"
#include "systems/mapPopulationSystem.h"
#include "systems/pauseMenuSystem.h"
#include "systems/mainMenuSystem.h"
#include "systems/mapTransitionSystem.h"
#include "systems/tileRenderer.h"
#include "systems/mapStateSystem.h"
#include "systems/mapFlowSystem.h"

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

	int currentMapId, lastMapId;
	int mapToDrawCount;
	Map* currentMap;
	std::unique_ptr<tson::Map> tiledMap;
	std::map<std::tuple<int, int>, tson::Tile*> tileData;
	list<Map> mapList;
	vector<int> openDoorsIds;
	vector<int> defeatedBossesIds;

	Hero* hero;
	KeyboardInput heroKeyboardInput;
	JoystickInput heroJoystickInput;

	LivingEntity* currentBoss = nullptr;

	list<Entity*> currentMapEnemies;
	list<Entity*> entities;
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
	bool isPaused = false;
	bool quit = false;
	bool mustSetBloodstainLocation = true;
	SessionStats sessionStats;

	HPBar* heroHpBar;
	HPBar* heroStBar;
	HPBar* bossHpBar;

	bool splashShowing;
	float overlayTimer;

	CameraController camController;
	TimeController gameTime;
	SoundManager soundManager;
	RenderContext renderContext;

	int mapQty;

	SaveHandler saveHandler;
	GameSaveManager gameSaveManager;
	NpcFactory npcFactory;
	TiledMapLoader tiledMapLoader;
	InteractionSystem* interactionSystem;
	MapPopulationSystem* mapPopulationSystem;
	PauseMenuSystem* pauseMenuSystem;
	MainMenuSystem* mainMenuSystem;
	MapTransitionSystem* mapTransitionSystem;
	TileRenderer* tileRenderer;
	MapStateSystem* mapStateSystem;
	MapFlowSystem* mapFlowSystem;

	QuickItemUi* quickItemUi;
	ItemPickMessageUi* itemPickMessageUi;
	ActionMessageUi* actionMessageUi;
	EssenceCounterUi* essenceCounterUi;
	MainMenu* mainMenu;
	PauseMenu* pauseMenu;

	GameState gameState;

	Game(SDL_Renderer* renderer);
	~Game();

	void update();
	void draw();
	void syncEntityRegistry();
	void handleInputCommand(const InputCommand& command);

private:
	SDL_Texture* gameCanvas = nullptr;
	int frameCount = 0;
	string resPath;

	class Bloodstain* bloodstain;

	bool isBossMap();

	void runMainMenu();
	void runMainGame();
	void runPausedGameMenu();
	void renderFrame();
	void updateEntities();
	void drawEntities();
	void drawMap();
	void updateMaps();
	void persistPickedMapItems();
	void loadTiledMap(const string& mapFile);
	void renderTiles();
	void handleMapChange(bool isHeroRespawn = false);
	void checkBossDeath();
	void spawnItem(int itemId, int quant, int xPos, int yPos);
	void saveCheckpointActivatedState(int checkpointId);
	// AnimationSets are initialized by entities themselves now.
	void spawnItemsFromCurrentMap();
	void inactivateCurrentMapItems();
	void removeAllEnemiesInMap();
	map<int, std::unique_ptr<Item>> loadInventoryItems(
		std::vector<std::pair<int, int>> items,
		SDL_Renderer* renderer);
	void saveGame(bool isCheckpointSave = false);
	void loadGame();
	GameState getGameState();
};

#endif // !GAME

