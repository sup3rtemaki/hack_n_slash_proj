#include "game.h"
#include "timeController.h"
#include "helpers/gameSaveManager.h"
#include "systems/interactionSystem.h"
#include "systems/mapPopulationSystem.h"

#include "npcs/door.h"

#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include "bloodstain.h"
#include "tileson/tileson.hpp"
#include "nlohmann/json.hpp"
#include "item/key.h"
#include "npcs/bosses/smallBrownSpider.h"
#include "checkpoint.h"
#include "npcs/npcFactory.h"
#include "resourceConfig.h"
#include "jsonFileStore.h"

using json = nlohmann::json;
using namespace std;

std::unique_ptr<tson::Map> tiledMap; // Tiled map
std::map<std::tuple<int, int>, tson::Tile*> tileData;

// Ant Hero maps are fixed-size square worlds; the camera controller receives this as a parameter.
const int WORLD_WIDTH = 1024;
const int WORLD_HEIGHT = 1024;

Game::Game() : gameSaveManager(saveHandler) {
	//TODO: Criar m�todo initialize ou algo do tipo pra encapsular tudo isso
	resPath = getResourcePath();
	entities.clear();

	mustSpawnEnemies = true;

	fadeImage = loadTexture(resPath + ResourcePaths::HUD_TEXTURES + "blackBG.png", Globals::renderer);
	splashImage = loadTexture(resPath + ResourcePaths::HUD_TEXTURES + "cyborgtitle.png", Globals::renderer);
	overlayImage = loadTexture(resPath + ResourcePaths::HUD_TEXTURES + "overlay.png", Globals::renderer);

	splashShowing = false;
	overlayTimer = 2;

	//setup camera
	Globals::camera.x = 0;
	Globals::camera.y = 0;
	Globals::camera.w = Globals::ScreenWidth;
	Globals::camera.h = Globals::ScreenHeight;

	//loadup sounds
	SoundManager::soundManager.loadSound(SoundIds::HIT, resPath + ResourcePaths::SOUNDS + "Randomize2.wav");
	SoundManager::soundManager.loadSound(SoundIds::ENEMY_HIT, resPath + ResourcePaths::SOUNDS + "Hit_Hurt9.wav");
	SoundManager::soundManager.loadSound(SoundIds::SWING, resPath + ResourcePaths::SOUNDS + "Randomize21.wav");
	SoundManager::soundManager.loadSound(SoundIds::DASH, resPath + ResourcePaths::SOUNDS + "dash.wav");
	SoundManager::soundManager.loadSound(SoundIds::GROWL, resPath + ResourcePaths::SOUNDS + "Randomize34.wav");
	SoundManager::soundManager.loadSound(SoundIds::ENEMY_DIE, resPath + ResourcePaths::SOUNDS + "Randomize41.wav");
	SoundManager::soundManager.loadSound(SoundIds::CRASH, resPath + ResourcePaths::SOUNDS + "crash.wav");
	SoundManager::soundManager.loadSound(SoundIds::SMASH, resPath + ResourcePaths::SOUNDS + "smash.wav");
	SoundManager::soundManager.loadSound(SoundIds::SHOOT, resPath + ResourcePaths::SOUNDS + "shoot2.wav");
	SoundManager::soundManager.loadSound(SoundIds::LAUGH, resPath + ResourcePaths::SOUNDS + "laugh2.wav");

	//song = Mix_LoadMUS(string(resPath + "Fatal Theory.wav").c_str());
	//if (song != NULL) {
	//	Mix_PlayMusic(song, -1);
	//	Mix_Volume(-1, 50);
	//}

	// AnimationSets: entities initialize their own sets when constructed.

	gameSaveManager.loadGame();
	currentMap = new Map();
	currentMap->file = gameSaveManager.getCurrentMapFile();

	loadTiledMap(resPath + ResourcePaths::MAPS + currentMap->file);

	// build hero entity
	hero = new Hero();
	hero->setSoundManager(&SoundManager::soundManager);
	hero->invincibleTimer = 0;
	hero->hp = gameSaveManager.getHeroHp();
	hero->x = hero->lastCheckpointPos.x = gameSaveManager.getHeroX();
	hero->y = hero->lastCheckpointPos.y = gameSaveManager.getHeroY();
	hero->essence = gameSaveManager.getEssence();
	hero->inventory.clear();
	hero->inventory = loadInventoryItems(gameSaveManager.getItems());
	//for (auto i : hero->inventory) {
	//	hero->addItemToQuickAccess(i.first);
	//	hero->quickAccessInventoryIndex++;
	//}
	hero->quickAccessInventoryIndex = 0;
	hero->lastCheckpointMapFile = currentMap->file;

	// open doors
	openDoorsIds = gameSaveManager.getOpenDoorsIds();
	defeatedBossesIds = gameSaveManager.getDefeatedBossesIds();

	// bloodstain
	bloodstain = new Bloodstain();
	bloodstain->setSoundManager(&SoundManager::soundManager);
	BloodstainInfo bloodstainInfo = gameSaveManager.getBloodstainInfo();
	bloodstain->setLocation(
		bloodstainInfo.x,
		bloodstainInfo.y,
		bloodstainInfo.essence,
		bloodstainInfo.mapName
	);
	if (bloodstain->x > 0 && bloodstain->y > 0 && !bloodstain->mapName.empty()) {
		bloodstain->isLive = true;
	}
	else {
		bloodstain->isLive = false;
	}

	if (bloodstain->mapName == currentMap->file) {
		bloodstain->create();
	}
	else {
		bloodstain->destroy();
	}
	entities.push_back(bloodstain);
	syncEntityRegistry();

	//keyboard
	hero->currentMap = currentMap;
	entities.push_back(hero);
	syncEntityRegistry();

	// Load NPC definitions and spawn a friendly NPC near the hero
	try {
		NpcFactory::getInstance().loadAllNpcs("data/npcs/");
		if (NpcFactory::getInstance().hasNpc("blacksmith")) {
			auto npcUP = NpcFactory::getInstance().createNpc("blacksmith");
			FriendlyNpc* npc = npcUP.release();
			// place npc close to hero
			npc->x = hero->x + 32;
			npc->y = hero->y;
			//npc->currentMap = currentMap;
			entities.push_back(npc);
			syncEntityRegistry();
			cout << "Spawned NPC: " << npc->getData().npcName << endl;
		}
	}
	catch (const std::exception& e) {
		cerr << "NPC spawn error: " << e.what() << std::endl;
	}

	// Initialize map population system BEFORE using it
	mapPopulationSystem = new MapPopulationSystem(
		tiledMap, currentMap, entities, walls, fogWalls, currentMapEnemies,
		hero, currentBoss, gui, openDoorsIds, defeatedBossesIds, deadEnemiesIds, bossHpBar
	);
	mapPopulationSystem->setSpawnItemCallback([this](int itemId, int quant, int xPos, int yPos) {
		this->spawnItem(itemId, quant, xPos, yPos);
	});
	mapPopulationSystem->setSyncRegistryCallback([this]() {
		this->syncEntityRegistry();
	});
	mapPopulationSystem->setIsBossMapCallback([this]() {
		return this->isBossMap();
	});

	// Populate the map
	mapPopulationSystem->buildWalls();
	mapPopulationSystem->buildWaypoints();
	mapPopulationSystem->buildDoors();
	mapPopulationSystem->spawnCheckpoints();
	mapPopulationSystem->spawnItemsFromCurrentMap();

	//get camera to follow hero
	camController.target = hero;

	gameCanvas = SDL_CreateTexture(
		Globals::renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET,
		Globals::ScreenWidth,
		Globals::ScreenHeight
	);

	quickItemUi = new QuickItemUi(hero);
	itemPickMessageUi = new ItemPickMessageUi(hero);
	actionMessageUi = new ActionMessageUi();
	essenceCounterUi = new EssenceCounterUi(hero);
	heroHpBar = new HPBar(hero, BarType::HERO_HEALTH_BAR);
	heroStBar = new HPBar(hero, BarType::HERO_STAMINA_BAR);
	hero->actionMessageUi = actionMessageUi;
	mainMenu = new MainMenu();
	pauseMenu = new PauseMenu(hero);

	// Initialize interaction system
	interactionSystem = new InteractionSystem(hero, actionMessageUi, openDoorsIds, mustRemoveAllEnemies, mustSpawnEnemies);
	interactionSystem->setSpawnItemCallback([this](int itemId, int quant, int xPos, int yPos) {
		this->spawnItem(itemId, quant, xPos, yPos);
	});
	interactionSystem->setSaveCheckpointCallback([this](int cpId) {
		this->saveCheckpointActivatedState(cpId);
	});

	gui.push_back(quickItemUi);
	gui.push_back(itemPickMessageUi);
	gui.push_back(actionMessageUi);
	gui.push_back(heroHpBar);
	gui.push_back(heroStBar);
	gui.push_back(essenceCounterUi);
	gui.push_back(pauseMenu);
	//TODO: descomentar quando implemetar rotina do menu
	//gui.push_back(gameMenu);

	buildBossNext = false;
	bossActive = false;

	camController.isLerping = true;

	updateMaps();
}

Game::~Game() {
	// Limpeza de texturas SDL
	cleanup(splashImage);
	cleanup(overlayImage);
	cleanup(fadeImage);

	if (scoreTexture != NULL) {
		cleanup(scoreTexture);
		scoreTexture = NULL;
	}

	// CORRIGIDO: Limpar texturas do cache
	texturesCache.clear();

	// Limpeza de �udio
	Mix_PausedMusic();
	if (song != nullptr) {
		Mix_FreeMusic(song);
		song = nullptr;
	}

	// ORDEM CR�TICA - MUITO IMPORTANTE:
	// 1� Deletar GUI (pode ter refer�ncias a entities)
	// 2� Deletar Entities (agora gerenciam seus AnimationSets)
	for (auto ui : gui) {
		delete ui;
	}
	gui.clear();

	// Delete systems
	if (interactionSystem != nullptr) {
		delete interactionSystem;
		interactionSystem = nullptr;
	}

	if (mapPopulationSystem != nullptr) {
		delete mapPopulationSystem;
		mapPopulationSystem = nullptr;
	}

	// PASSO 2: Limpar todas as listas de entities
	Entity::removeAllFromList(&walls, false);
	Entity::removeAllFromList(&currentMapEnemies, false);
	Entity::removeAllFromList(&fogWalls, false);

	// CR�TICO: Mudar de 'false' para 'true' para deletar entities
	Entity::removeAllFromList(&entities, true);
	Entity::entities = entities;

	deadEnemiesIds.clear();
	openDoorsIds.clear();
	defeatedBossesIds.clear();

	// Game no longer holds AnimationSet pointers; individual entities clean up their own.

	// Limpeza de mapa
	if (currentMap != nullptr) {
		delete currentMap;
		currentMap = nullptr;
	}

	// Resetar ponteiros (j� foram deletados em Entity::entities)
	currentBoss = nullptr;
	bloodstain = nullptr;
	hero = nullptr;

	// Limpar unique_ptr
	tiledMap.reset();

	if (gameCanvas) {
		SDL_DestroyTexture(gameCanvas);
	}
}

void Game::syncEntityRegistry() {
	Entity::setActiveWorld(&entities);
	Entity::entities = entities;
}

void Game::handleInputCommand(const InputCommand& command) {
	if (command.type == InputCommandType::Move) {
		if (command.source == InputSource::Keyboard && hero->isMovingMethod == 2) {
			return;
		}
		if (command.source == InputSource::Keyboard) {
			hero->isMovingMethod = 1;
		}
		else {
			hero->isMovingMethod = 2;
		}
		hero->move(command.angle);
		return;
	}

	if (command.type == InputCommandType::Stop) {
		if ((command.source == InputSource::Keyboard && hero->isMovingMethod != 2) ||
			(command.source == InputSource::Joystick && hero->isMovingMethod != 1)) {
			hero->moving = false;
			hero->isMovingMethod = 0;
		}
		return;
	}

	switch (command.type) {
	case InputCommandType::Dash:
		hero->dash();
		break;
	case InputCommandType::Attack:
		hero->attack();
		break;
	case InputCommandType::UseItem:
		hero->useSelectedItemQuickAccess();
		break;
	case InputCommandType::Action:
		hero->takeAction();
		break;
	case InputCommandType::NextQuickItem:
		hero->quickAccessInventoryIndex++;
		if (hero->quickAccessInventory[hero->quickAccessInventoryIndex] == -1) {
			hero->quickAccessInventoryIndex = 0;
		}
		break;
	default:
		break;
	}
}

void Game::update() {
	// setup time controller before game starts
	gameTime.reset();
	gameState = GameState::MainMenu;

	while (!quit) {
		switch (gameState) {
		case GameState::MainMenu:
			runMainMenu();
			break;
		case GameState::Loading:
			break;
		case GameState::InGame:
			runMainGame();
			break;
		case GameState::Paused:
			runPausedGameMenu();
			break;
		case GameState::None:
			gameState = GameState::MainMenu;
			break;
		}
	}
	//game loop
}

bool Game::isBossMap() {
	auto tMap = tiledMap.get();
	if (tMap == nullptr) {
		cout << "Mapa nulo" << endl;
		return false;
	}

	auto layer = tMap->getLayer("BossSpawn");
	if (layer == nullptr) return false;

	// Aqui pode ter um problema futuramente, pois assumimos que s� h� um objeto de boss na layer
	for (auto obj : layer->getObjects()) {
		int bossId = std::any_cast<int>(obj.getProp("bossId")->getValue());
		std::vector<int>::iterator it;
		it = find(defeatedBossesIds.begin(), defeatedBossesIds.end(), bossId);
		if (it != defeatedBossesIds.end()) {
			return false;
		}
	}

	return true;
}

void Game::runMainMenu() {
	while (SDL_PollEvent(&event)) {
		// close the window
		if (event.type == SDL_QUIT) {
			quit = true;
		}

		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.scancode) {
			case SDL_SCANCODE_UP:
				mainMenu->onIndexUp();
				break;
			case SDL_SCANCODE_DOWN:
				mainMenu->onIndexDown();
				break;
			case SDL_SCANCODE_ESCAPE:
				quit = true;
				break;
			case SDL_SCANCODE_SPACE:
				switch (mainMenu->index) {
				case 0:
					gameState = GameState::InGame;
					Globals::pause = false;
					break;
				case 1:
					quit = true;
					break;
				}
			}
		}
	}

	SDL_SetRenderDrawColor(Globals::renderer, 145, 133, 129, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(Globals::renderer);
	mainMenu->draw();
	SDL_RenderPresent(Globals::renderer);
}

void Game::runMainGame() {
	frameCount++;

	if (frameCount % 60 == 0) {
		cout << frameCount << endl;
		frameCount = 0;
	}

	gameTime.updateTime();

	Entity::removeInactiveEntitiesFromList(&entities, false);
	syncEntityRegistry();

	LivingEntity::saveDeadEnemiesIds(currentMapEnemies, deadEnemiesIds);

	//remove and delete enemies in the list who are dead/inactive
	Entity::removeInactiveEntitiesFromList(&currentMapEnemies, true);

	// check for any events that might have happened
	while (SDL_PollEvent(&event)) {
		// close the window
		if (event.type == SDL_QUIT) {
			quit = true;
		}

		// keydown event
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.scancode) {
			case SDL_SCANCODE_ESCAPE:
				pauseMenu->menuState = MenuState::Active;
				Globals::pause = true;
				gameState = GameState::Paused;
				break;
			case SDL_SCANCODE_SPACE:
				if (splashShowing) {
					splashShowing = false;
				}

				if (overlayTimer <= 0 && hero->hp < 1) {
					//cleanup and restart game
					overlayTimer = 2;
					enemyWavesTillBoss = 3;
					bossActive = false;
					buildBossNext = false;
					//bossHpBar->entity = nullptr; // make hpbar point to no entities

					RoundKing::roundKingsKilled = 0;
					Glob::globsKilled = 0;
					Grob::grobsKilled = 0;

					if (scoreTexture != NULL) {
						cleanup(scoreTexture);
						scoreTexture = NULL;
					}

					deadEnemiesIds.clear();

					//remove existing enemies
					for (list<Entity*>::iterator enemy = currentMapEnemies.begin(); enemy != currentMapEnemies.end(); enemy++) {
						(*enemy)->active = false;
					}

					mustSetBloodstainLocation = true;
					mustSpawnEnemies = true;
					handleMapChange(true);
				}

				break;
			}
		}
		if (!isFading) {
			for (const auto& command : heroKeyboardInput.update(&event)) {
				handleInputCommand(command);
			}
			for (const auto& command : heroJoystickInput.update(&event)) {
				handleInputCommand(command);
			}
		}
		else {
			hero->moving = false;
		}
	}

	if (hero->mustUpdateKeyJoyInput) {
		for (const auto& command : heroKeyboardInput.update(&event)) {
			handleInputCommand(command);
		}
		for (const auto& command : heroJoystickInput.update(&event)) {
			handleInputCommand(command);
		}
		hero->mustUpdateKeyJoyInput = false;
	}

	// joystick axis must be updated outside the poll event loop because of how the
	// interaction with the axis works. consider refactoring in the future
	for (const auto& command : heroJoystickInput.checkAxis()) {
		handleInputCommand(command);
	}

	if (hero->hp < 1) {
		if (overlayTimer > 0) {
			overlayTimer -= gameTime.dT; //make overlay timer tick down
		}
		else {
			if (mustSetBloodstainLocation) {
				bloodstain->setLocation(
					hero->x,
					hero->y,
					hero->essence,
					currentMap->file);
				hero->essence = 0;
				mustSetBloodstainLocation = false;
			}
		}
	}

	//handle remove/spawn enemies
	if (hero->hp > 0 && !splashShowing) {
		if (mustRemoveAllEnemies) {
			mustRemoveAllEnemies = false;
			removeAllEnemiesInMap();
		}
		if (currentMapEnemies.size() <= 0 && mustSpawnEnemies) {
			// Only spawn boss if not already spawned
			if (currentBoss == nullptr) {
				mapPopulationSystem->spawnBoss();
			}
			mapPopulationSystem->spawnEnemies();
			mustSpawnEnemies = false;  // Prevent spawning again
		}
	}

	// update all entities
	if (!Globals::pause) {
		updateEntities();
	}

	//If hero is in change map region, fade to change map
	for (auto& waypoint : currentMap->currentMapWaypoints) {
		if ((hero->x > waypoint.waypointRect.x) &&
			(hero->x < waypoint.waypointRect.x + waypoint.waypointRect.w) &&
			(hero->y > waypoint.waypointRect.y) &&
			(hero->y < waypoint.waypointRect.y + waypoint.waypointRect.h)) {
			currentMap->nextMapWaypoint = waypoint;
			isFading = true;
			fadeIn = true;
		}
	}

	if (hero->mustSaveGame) {
		hero->mustSaveGame = false;
		hero->lastCheckpointMapFile = currentMap->file;
		saveGame(true);
	}

	renderFrame();

	// framerate
	// cout << gameTime.dT << endl;
}

void Game::updateEntities() {
	for (list<Entity*>::iterator entity = entities.begin(); entity != entities.end(); entity++) {
		(*entity)->deltaTime = gameTime.dT;
		(*entity)->update();

		interactionSystem->processEntity(*entity);
	}
}

void Game::drawEntities() {
	entities.sort(Entity::EntityCompare);
	syncEntityRegistry();

	for (list<Entity*>::iterator entity = entities.begin(); entity != entities.end(); entity++) {
		(*entity)->draw(renderContext);
	}
}

void Game::drawMap() {
	renderTiles();
	renderTexture(fadeImage, Globals::renderer, (-200) - Globals::camera.x, (-200) - Globals::camera.y);
}

void Game::runPausedGameMenu() {
	gameTime.updateTime();

	// check for any events that might have happened
	while (SDL_PollEvent(&event)) {
		// close the window
		if (event.type == SDL_QUIT) {
			quit = true;
		}

		// keydown event
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.scancode) {
			case SDL_SCANCODE_ESCAPE:
				// Verifica se est� no modo de sele��o de quick slot
				if (pauseMenu->inventoryMode == InventoryMode::SelectingQuickSlot) {
					pauseMenu->cancelQuickSlotSelection();
					break;
				}

				switch (pauseMenu->menuState) {
				case MenuState::Active:
					pauseMenu->menuState = MenuState::Inactive;
					Globals::pause = false;
					gameState = GameState::InGame;
					break;
				case MenuState::Background:
					pauseMenu->hideSubMenu();
					break;
				}
				break;

			case SDL_SCANCODE_SPACE:
				// Verifica se est� no modo de sele��o de quick slot
				if (pauseMenu->inventoryMode == InventoryMode::SelectingQuickSlot) {
					pauseMenu->confirmQuickSlotSelection();
					break;
				}

				switch (pauseMenu->currentPage) {
				case (MenuPage::PAGE1):
					break;
				case (MenuPage::PAGE2):
					if (pauseMenu->menuState == MenuState::Active) {
						pauseMenu->showSubMenu();
					}
					else if (pauseMenu->menuState == MenuState::Background) {
						pauseMenu->onSubMenuAction();
					}
					break;
				}
				break;
			case SDL_SCANCODE_UP:
				pauseMenu->onIndexUp();
				break;
			case SDL_SCANCODE_DOWN:
				pauseMenu->onIndexDown();
				break;
			case SDL_SCANCODE_LEFT:
				pauseMenu->onIndexLeft();
				break;
			case SDL_SCANCODE_RIGHT:
				pauseMenu->onIndexRight();
				break;
			case SDL_SCANCODE_1:
				if (pauseMenu->menuState == MenuState::Active) {
					pauseMenu->currentPage = MenuPage::PAGE1;
				}
				break;
			case SDL_SCANCODE_2:
				pauseMenu->currentPage = MenuPage::PAGE2;
				break;
			}
		}
		if (!isFading) {
			for (const auto& command : heroKeyboardInput.update(&event)) {
				handleInputCommand(command);
			}
			for (const auto& command : heroJoystickInput.update(&event)) {
				handleInputCommand(command);
			}
		}
		else {
			hero->moving = false;
		}
	}

	//if (hero->mustUpdateKeyJoyInput) {
	//	heroKeyboardInput.update(&event);
	//	heroJoystickInput.update(&event);
	//	hero->mustUpdateKeyJoyInput = false;
	//}

	// joystick axis must be updated outside the poll event loop because of how the
	// interaction with the axis works. consider refactoring in the future
	//heroJoystickInput.checkAxis();

	renderFrame();
}

void Game::renderFrame() {
	// update map state before drawing the frame
	updateMaps();

	// draw all entites
	draw();

	// update camera position
	camController.deltaTime = gameTime.dT;
	camController.update(Globals::camera, WORLD_WIDTH, WORLD_HEIGHT);
}

void Game::updateMaps() {
	if (isFading) {
		hero->moving = false;

		if (alpha < 255 && fadeIn) {
			fadeIn = true;
			fadeOut = false;
			alphaCalc += 15.0f;
			alpha = alphaCalc;
			SDL_SetTextureAlphaMod(fadeImage, alpha);

			if (alpha > 254) {
				//camController.isLerping = false;

				inactivateCurrentMapItems();

				// TODO: Levar essa rotina de atualizar o status do item no arquivo json
				// pra outro lugar, e tentar melhorar pq ta muito ruim e feio
				const string itemsMapFilePath = getResourcePath() + ResourcePaths::MAPS + currentMap->file;
				json mapFile;
				if (JsonFileStore::readJsonFile(itemsMapFilePath, mapFile) == JsonFileResult::Success) {

				for (auto const& i : currentMap->itemsInMap) {
					if (i.first) {
						for (auto& layersIt : mapFile["layers"]) {
							string name = layersIt["name"];
							if (name == "Items") {
								for (auto& object : layersIt["objects"]) {
									if ((int)object["x"] == std::get<1>(i.second) &&
										(int)object["y"] == std::get<2>(i.second)) {
										for (auto& prop : object["properties"]) {
											if (prop["name"] == "itemId" &&
												prop["value"] == std::get<0>(i.second)) {
												for (auto& prop2 : object["properties"]) {
													if (prop2["name"] == "isPicked") {
														prop2["value"].clear();
														prop2["value"] = true;
														JsonFileStore::writeJsonFile(itemsMapFilePath, mapFile);
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
				}

				handleMapChange();
			}
		}
		else if (alpha >= 0 && fadeOut) {
			fadeIn = false;
			fadeOut = true;
			alphaCalc -= 10.0f;
			alpha = alphaCalc;
			SDL_SetTextureAlphaMod(fadeImage, alpha);

			if (alpha == 0) {
				fadeOut = false;
			}
		}
		else {
			isFading = false;
			fadeIn = false;
			fadeOut = false;
		}
	}
	else {
		camController.isLerping = true;
		SDL_SetTextureAlphaMod(fadeImage, 0);
		alpha = 0;
		alphaCalc = 0.0f;
	}

	checkBossDeath();
}

void Game::loadTiledMap(const string& mapFile) {
	tiledMap = tiledMapLoader.load(mapFile);
}

void Game::renderTiles() {
	auto currentMap = tiledMap.get();
	if (currentMap == nullptr) {
		cout << "Mapa nulo" << endl;
		return;
	}

	string tilesetName;
	string tilesetTexturePath;
	int x = 0;
	int y = 0;
	SDL_Texture* texture = nullptr; // Continua usando raw pointer local

	for (auto layer : currentMap->getLayers()) {
		if (&layer == nullptr) {
			cout << "layer null" << endl;
		}

		for (auto& [pos, tileObject] : layer.getTileObjects()) {
			if (tileObject.getTile() == nullptr) {
				cout << "tile null" << endl;
				return;
			}

			if (layer.getType() == tson::LayerType::TileLayer) {
				tson::Tileset* tileset = tileObject.getTile()->getTileset();
				bool hasAnimation = tileObject.getTile()->getAnimation().any();
				tson::Rect drawingRect;

				bool isTransparentTile = std::any_cast<bool>(
					tileObject.getTile()->getProp("isTransparent")->getValue());

				if (!isTransparentTile) {
					if (!hasAnimation) {
						drawingRect = tileObject.getDrawingRect();
					}

					tilesetName = tileset->getImage().filename().string();

					// MUDAN�A AQUI: Buscar no map de unique_ptr
					if (auto search = texturesCache.find(tilesetName);
						search != texturesCache.end()) {
						// Usar .get() para pegar o raw pointer
						texture = search->second.get();
					}
					else {
						// Carregar nova textura
						tilesetTexturePath = resPath + ResourcePaths::TEXTURES +
							tileset->getImage().filename().string();

						SDL_Texture* newTexture = loadTexture(tilesetTexturePath,
							Globals::renderer);

						// Criar unique_ptr e mover para o cache
						TexturePtr texPtr(newTexture, TextureDeleter{});
						texture = texPtr.get();

						texturesCache.emplace(tilesetName, std::move(texPtr));
					}

					if (texture == nullptr) {
						cout << "texture null " << tilesetName << endl;
						return;
					}

					// ... resto do c�digo de renderiza��o igual
					SDL_Rect tileRect;
					tileRect.x = tileObject.getDrawingRect().x;
					tileRect.y = tileObject.getDrawingRect().y;
					tileRect.w = tileObject.getDrawingRect().width;
					tileRect.h = tileObject.getDrawingRect().height;

					SDL_Rect renderTile;
					renderTile.x = (int)((x * tileRect.w) - Globals::camera.x);
					renderTile.y = (int)((y * tileRect.h) - Globals::camera.y);
					renderTile.w = tileRect.w;
					renderTile.h = tileRect.h ;

					SDL_RenderCopy(Globals::renderer, texture, &tileRect, &renderTile);
				}

				y++;
				if (y >= 32) {
					y = 0;
					x++;
					if (x >= 32) {
						x = 0;
					}
				}
			}
		}
	}
}


void Game::handleMapChange(bool isHeroRespawn) {
	if (isHeroRespawn) {
		saveGame(true);
		hero->revive();
		loadGame();
		currentMap->file = gameSaveManager.getCurrentMapFile();
		hero->hp = gameSaveManager.getHeroHp();
		hero->x = gameSaveManager.getHeroX();
		hero->y = gameSaveManager.getHeroY();
	}
	else {
		currentMap->file = currentMap->nextMapWaypoint.nextMapFile;
		hero->x = currentMap->nextMapWaypoint.xDestination;
		hero->y = currentMap->nextMapWaypoint.yDestination;

		fadeIn = false;
		fadeOut = true;
	}

	hero->attackBuffer.clear();
	loadTiledMap(resPath + ResourcePaths::MAPS + currentMap->file);

	// Remove enemies
	for (list<Entity*>::iterator enemy = currentMapEnemies.begin(); enemy != currentMapEnemies.end(); enemy++) {
		(*enemy)->active = false;
	}

	// Remove fog walls explicitly
	for (list<Entity*>::iterator fogWall = fogWalls.begin(); fogWall != fogWalls.end(); fogWall++) {
		(*fogWall)->active = false;
	}
	fogWalls.clear();

	// Remove walls, doors and checkpoints
	for (list<Entity*>::iterator entity = entities.begin(); entity != entities.end(); entity++) {
		if ((*entity)->type == "wall" ||
				(*entity)->type == "door" ||
				(*entity)->type == "checkpoint" ||
				(*entity)->type == "boss") {
			(*entity)->active = false;
		}
	}
	syncEntityRegistry();

	currentBoss = nullptr;

	mustSpawnEnemies = true;
	hero->currentMap = currentMap;
	mapPopulationSystem->spawnItemsFromCurrentMap();
	mapPopulationSystem->buildWalls();
	mapPopulationSystem->buildWaypoints();
	mapPopulationSystem->buildDoors();
	mapPopulationSystem->spawnCheckpoints();
	if (bloodstain->isLive &&
		currentMap->file == bloodstain->mapName) {
		bloodstain->create();
	}
	else {
		bloodstain->destroy();
	}
	openDoorsIds = {};
}


void Game::draw() {
	renderContext.renderer = Globals::renderer;
	renderContext.camera = Globals::camera;
	renderContext.debugging = Globals::debugging;

	// 1. Tudo o que for desenhado agora vai para o gameCanvas
	SDL_SetRenderTarget(Globals::renderer, gameCanvas);

	// 2. Limpa o canvas virtual
	SDL_SetRenderDrawColor(Globals::renderer, 20, 20, 20, 255); // Cor de fundo
	SDL_RenderClear(Globals::renderer);

	// clear screen
	//SDL_SetRenderDrawColor(Globals::renderer, 145, 133, 129, SDL_ALPHA_OPAQUE);
	//SDL_RenderClear(Globals::renderer);

	if (splashShowing) {
		renderTexture(splashImage, Globals::renderer, 0, 0);
	}
	else {
		drawMap();

		// draw all of the entities
		drawEntities();

		// draw all of the UI
		for (list<Ui*>::iterator ui = gui.begin(); ui != gui.end(); ui++) {
			(*ui)->deltaTime = gameTime.dT;
			(*ui)->draw();
		}

		if (overlayTimer <= 0 && hero->hp < 1) {
			renderTexture(overlayImage, Globals::renderer, 0, 0);

			if (scoreTexture == NULL) {
				//generate score text
				SDL_Color color = { 255, 255, 255, 255 };

				stringstream ss;
				ss << "Enemies dispatched: " << Glob::globsKilled + Grob::grobsKilled + RoundKing::roundKingsKilled;

				scoreTexture = renderText(ss.str(), resPath + ResourcePaths::FONTS + "vermin_vibes_1989.ttf", color, 30, Globals::renderer);
			}

			renderTexture(scoreTexture, Globals::renderer, 20, 50);
		}
	}

	// 4. Volta o alvo para a tela real (janela)
	SDL_SetRenderTarget(Globals::renderer, NULL);

	// 5. Limpa a tela real (opcional, mas boa pr�tica)
	SDL_RenderClear(Globals::renderer);

	// 6. Desenha o canvas inteiro na tela de uma vez s�
	// Como o LogicalSize est� ativo no main.cpp, o SDL vai esticar
	// o canvas perfeitamente para preencher a janela.
	SDL_RenderCopy(Globals::renderer, gameCanvas, NULL, NULL);
	SDL_SetRenderDrawColor(Globals::renderer, 20, 20, 20, 255);

	// 7. Apresenta o frame
	SDL_RenderPresent(Globals::renderer);
	// after done drawing, show it to the screen
	//SDL_RenderPresent(Globals::renderer);
}


void Game::spawnItem(int itemId, int quant, int xPos, int yPos) {
	//TODO: Spawnar itens usando a lista currentMap->itemsInMap
	//TODO: Criar switch case com enums do itemId
	Item* spawnItem;
	bool canSpawn = true;

	switch (itemId) {
	case Item::HONEYDEW_POTION_ID:
		spawnItem = new HoneydewPotion(canSpawn, quant);
		break;
	case Item::GREEN_BERRY_ID:
		spawnItem = new GreenBerry(canSpawn, quant);
		break;
	case Item::STONE_ID:
		spawnItem = new Stone(canSpawn, quant);
		break;
	default:
		return;
	}

	spawnItem->x = xPos;
	spawnItem->y = yPos;
	spawnItem->active = true;
	entities.push_back(spawnItem);
	syncEntityRegistry();
}

void Game::checkBossDeath() {
	if (currentBoss != nullptr && currentBoss->active && currentBoss->hp <= 0) {
		std::vector<int>::iterator it;
		it = find(defeatedBossesIds.begin(), defeatedBossesIds.end(), currentBoss->id);
		if (it == defeatedBossesIds.end()) {
			defeatedBossesIds.push_back(currentBoss->id);
		}

		for (auto fogWall : fogWalls) {
			fogWall->active = false;
		}

		fogWalls.clear();
		LootDropSource* lootSource = dynamic_cast<LootDropSource*>(currentBoss);
		if (lootSource != nullptr && !lootSource->hasDroppedEssence()) {
			hero->addEssence(lootSource->getEssence());
			lootSource->markEssenceDropped();
		}
		// currentBoss = nullptr;
	}
}

void Game::saveCheckpointActivatedState(int checkpointId) {
	const string mapFilePath = getResourcePath() + ResourcePaths::MAPS + currentMap->file;
	json mapFile;
	if (JsonFileStore::readJsonFile(mapFilePath, mapFile) != JsonFileResult::Success) {
		return;
	}
	bool checkpointFound = false;

	for (auto& layersIt : mapFile["layers"]) {
		string name = layersIt["name"];
		if (name == "Checkpoints") {
			for (auto& object : layersIt["objects"]) {
				for (auto& prop : object["properties"]) {
					if (prop["name"] == "checkpointId" &&
						prop["value"] == checkpointId) {
						for (auto& prop2 : object["properties"]) {
							if (prop2["name"] == "isActive") {
								prop2["value"].clear();
								prop2["value"] = true;
								JsonFileStore::writeJsonFile(mapFilePath, mapFile);
							}
						}
					}
				}
			}
		}
	}
}

// loadAnimationSets was removed: entities initialize their own AnimationSets now.

void Game::inactivateCurrentMapItems() {
	for (list<Entity*>::iterator entity = entities.begin(); entity != entities.end(); entity++) {
		if (dynamic_cast<Item*>((*entity)) != nullptr) {
			Item* i = (Item*)(*entity);
			i->active = false;
		}
	}
	syncEntityRegistry();
}

void Game::removeAllEnemiesInMap() {
	for (list<Entity*>::iterator enemy = currentMapEnemies.begin(); enemy != currentMapEnemies.end(); enemy++) {
		(*enemy)->active = false;
	}
	Entity::removeInactiveEntitiesFromList(&entities, false);
	syncEntityRegistry();
	Entity::removeInactiveEntitiesFromList(&currentMapEnemies, true);
	currentMapEnemies.clear();
	deadEnemiesIds.clear();
}

map<int, std::unique_ptr<Item>> Game::loadInventoryItems(std::vector<std::pair<int, int>> items) {
	return gameSaveManager.loadInventoryItems(items);
}

void Game::saveGame(bool isCheckpointSave) {
	gameSaveManager.saveGame(hero, currentMap, bloodstain, openDoorsIds, defeatedBossesIds, isCheckpointSave);
}

void Game::loadGame() {
	if (!gameSaveManager.loadGame()) {
		cerr << "Erro ao carregar savefile" << endl;
		terminate();
	}
}

GameState Game::getGameState() {
	return gameState;
}
