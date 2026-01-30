#include "game.h"

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

using json = nlohmann::json;
using namespace std;

tson::Tileson tileson; // Tileson global instance
std::unique_ptr<tson::Map> tiledMap; // Tiled map
std::map<std::tuple<int, int>, tson::Tile*> tileData;

const string MAPS_FOLDER_PATH = "Maps\\";
const string TEXTURES_FOLDER_PATH = "Assets\\Textures\\";
const string SOUNDS_FOLDER_PATH = "Assets\\Sounds\\";
const string ANIMATIONS_FOLDER_PATH = "Assets\\Animations\\";

Game::Game() {
	//TODO: Criar método initialize ou algo do tipo pra encapsular tudo isso
	resPath = getResourcePath();

	mustSpawnEnemies = true;

	fadeImage = loadTexture(resPath + Ui::HUD_TEXTURES_PATH + "blackBG.png", Globals::renderer);
	splashImage = loadTexture(resPath + Ui::HUD_TEXTURES_PATH + "cyborgtitle.png", Globals::renderer);
	overlayImage = loadTexture(resPath + Ui::HUD_TEXTURES_PATH + "overlay.png", Globals::renderer);

	splashShowing = false;
	overlayTimer = 2;

	//setup camera
	Globals::camera.x = 0;
	Globals::camera.y = 0;
	Globals::camera.w = Globals::ScreenWidth;
	Globals::camera.h = Globals::ScreenHeight;

	//loadup sounds
	SoundManager::soundManager.loadSound("hit", resPath + SOUNDS_FOLDER_PATH + "Randomize2.wav");
	SoundManager::soundManager.loadSound("enemyHit", resPath + SOUNDS_FOLDER_PATH + "Hit_Hurt9.wav");
	SoundManager::soundManager.loadSound("swing", resPath + SOUNDS_FOLDER_PATH + "Randomize21.wav");
	SoundManager::soundManager.loadSound("dash", resPath + SOUNDS_FOLDER_PATH + "dash.wav");
	SoundManager::soundManager.loadSound("growl", resPath + SOUNDS_FOLDER_PATH + "Randomize34.wav");
	SoundManager::soundManager.loadSound("enemyDie", resPath + SOUNDS_FOLDER_PATH + "Randomize41.wav");
	SoundManager::soundManager.loadSound("crash", resPath + SOUNDS_FOLDER_PATH + "crash.wav");
	SoundManager::soundManager.loadSound("smash", resPath + SOUNDS_FOLDER_PATH + "smash.wav");
	SoundManager::soundManager.loadSound("shoot", resPath + SOUNDS_FOLDER_PATH + "shoot2.wav");
	SoundManager::soundManager.loadSound("laugh", resPath + SOUNDS_FOLDER_PATH + "laugh2.wav");

	//song = Mix_LoadMUS(string(resPath + "Fatal Theory.wav").c_str());
	//if (song != NULL) {
	//	Mix_PlayMusic(song, -1);
	//	Mix_Volume(-1, 50);
	//}

	//TODO: Levar essa rotina pro loadGame
	loadAnimationSets();

	saveHandler.load();
	currentMap = new Map();
	currentMap->file = saveHandler.getCurrentMapFile();

	loadTiledMap(resPath + MAPS_FOLDER_PATH + currentMap->file);

	// build hero entity
	hero = new Hero(heroAnimSet.get());
	hero->invincibleTimer = 0;
	hero->hp = saveHandler.getHeroHp();
	hero->x = hero->lastCheckpointPos.x = saveHandler.getHeroX();
	hero->y = hero->lastCheckpointPos.y = saveHandler.getHeroY();
	hero->essence = saveHandler.getEssence();
	hero->inventory.clear();
	hero->inventory = loadInventoryItems(saveHandler.getItems());
	//for (auto i : hero->inventory) {
	//	hero->addItemToQuickAccess(i.first);
	//	hero->quickAccessInventoryIndex++;
	//}
	hero->quickAccessInventoryIndex = 0;
	hero->lastCheckpointMapFile = currentMap->file;

	// open doors
	openDoorsIds = saveHandler.getOpenDoorsIds();
	defeatedBossesIds = saveHandler.getDefeatedBossesIds();

	// bloodstain
	bloodstain = new Bloodstain(bloodstainAnimSet.get());
	BloodstainInfo bloodstainInfo = saveHandler.getBloodstainInfo();
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
	Entity::entities.push_back(bloodstain);

	//keyboard
	heroKeyboardInput.hero = hero;
	heroJoystickInput.hero = hero;
	hero->currentMap = currentMap;
	Entity::entities.push_back(hero);

	buildWalls();
	buildWaypoints();
	buildDoors();
	spawnCheckpoints();

	spawnItemsFromCurrentMap();

	//get camera to follow hero
	camController.target = hero;

	quickItemUi = new QuickItemUi(hero);
	itemPickMessageUi = new ItemPickMessageUi(hero);
	actionMessageUi = new ActionMessageUi();
	essenceCounterUi = new EssenceCounterUi(hero);
	heroHpBar = new HPBar(hero, BarType::HERO_HEALTH_BAR);
	heroStBar = new HPBar(hero, BarType::HERO_STAMINA_BAR);
	hero->actionMessageUi = actionMessageUi;
	mainMenu = new MainMenu();
	pauseMenu = new PauseMenu(hero);

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

	// teste
	spawnBoss();
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

	// Limpeza de áudio
	Mix_PausedMusic();
	if (song != nullptr) {
		Mix_FreeMusic(song);
		song = nullptr;
	}

	// ORDEM CRÍTICA - MUITO IMPORTANTE:
	// 1º Deletar GUI (pode ter referências a entities)
	// 2º Deletar Entities (usam AnimationSets)
	// 3º Deletar AnimationSets

	// PASSO 1: Limpar GUI
	for (auto ui : gui) {
		delete ui;
	}
	gui.clear();

	// PASSO 2: Limpar todas as listas de entities
	Entity::removeAllFromList(&walls, false);
	Entity::removeAllFromList(&currentMapEnemies, false);
	Entity::removeAllFromList(&fogWalls, false);

	// CRÍTICO: Mudar de 'false' para 'true' para deletar entities
	Entity::removeAllFromList(&Entity::entities, true);

	deadEnemiesIds.clear();
	openDoorsIds.clear();
	defeatedBossesIds.clear();

	// Boa prática: Setar nullptr após delete
	heroAnimSet = nullptr;
	globAnimSet = nullptr;
	grobAnimSet = nullptr;
	termiteMinerAnimSet = nullptr;
	wallAnimSet = nullptr;
	roundKingAnimSet = nullptr;
	bulletAnimSet = nullptr;
	stoneProjectileAnimSet = nullptr;
	smallBrownSpiderAnimSet = nullptr;
	doubleDoorsAnimSet = nullptr;
	checkpointAnimSet = nullptr;
	bloodstainAnimSet = nullptr;
	hDewPotionAnimSet = nullptr;

	// Limpeza de mapa
	if (currentMap != nullptr) {
		delete currentMap;
		currentMap = nullptr;
	}

	// Resetar ponteiros (já foram deletados em Entity::entities)
	currentBoss = nullptr;
	bloodstain = nullptr;
	hero = nullptr;

	// Limpar unique_ptr
	tiledMap.reset();
}

void Game::update() {
	// setup time controller before game starts
	TimeController::timeController.reset();
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

	// Aqui pode ter um problema futuramente, pois assumimos que só há um objeto de boss na layer
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

bool Game::isLivingEntityDead(Entity* entity) {
	if (entity->active &&
		dynamic_cast<LivingEntity*>(entity) != nullptr &&
		((LivingEntity*)entity)->hp <= 0) {
		return true;
	}
	return false;
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
	TimeController::timeController.updateTime();

	Entity::removeInactiveEntitiesFromList(&Entity::entities, false);

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
			heroKeyboardInput.update(&event);
			heroJoystickInput.update(&event);
		}
		else {
			hero->moving = false;
		}
	}

	if (hero->mustUpdateKeyJoyInput) {
		heroKeyboardInput.update(&event);
		heroJoystickInput.update(&event);
		hero->mustUpdateKeyJoyInput = false;
	}

	// joystick axis must be updated outside the poll event loop because of how the
	// interaction with the axis works. consider refactoring in the future
	heroJoystickInput.checkAxis();

	if (hero->hp < 1) {
		if (overlayTimer > 0) {
			overlayTimer -= TimeController::timeController.dT; //make overlay timer tick down
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
			spawnBoss();
			spawnEnemies();
		}
	}

	// update all entites
	if (!Globals::pause) {
		for (list<Entity*>::iterator entity = Entity::entities.begin(); entity != Entity::entities.end(); entity++) {
			// update all entites in world at once (polymorphism)
			(*entity)->update();

			checkAndHandleEnemyLoot(*entity);
			checkAndHandleNearItem(*entity);
			checkAndHandleNearDoor(*entity);
			checkAndHandleNearBloodstain(*entity);
			checkAndHandleNearCheckpoint(*entity);
		}
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

	// draw all entites
	draw();

	// update camera position
	camController.update();

	// framerate
	// cout << TimeController::timeController.dT << endl;
}

void Game::runPausedGameMenu() {
	TimeController::timeController.updateTime();

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
				// Verifica se está no modo de seleção de quick slot
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
				// Verifica se está no modo de seleção de quick slot
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
			heroKeyboardInput.update(&event);
			heroJoystickInput.update(&event);
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

	// draw all entites
	draw();

	// update camera position
	camController.update();
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
				std::ifstream ifs(getResourcePath() + MAPS_FOLDER_PATH + currentMap->file);
				json mapFile = json::parse(ifs);

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
														std::ofstream outputFile(getResourcePath() + MAPS_FOLDER_PATH + currentMap->file);
														prop2["value"].clear();
														prop2["value"] = true;
														outputFile << std::setw(4) << mapFile << std::endl;
														outputFile.close();
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
	renderTiles();
	renderTexture(fadeImage, Globals::renderer, (-200) - Globals::camera.x, (-200) - Globals::camera.y);
}

void Game::loadTiledMap(const string& mapFile) {
	tiledMap = tileson.parse(fs::path(mapFile));
	if (tiledMap->getStatus() != tson::ParseStatus::OK) {
		cout << tiledMap->getStatusMessage() << endl;
	}
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

					// MUDANÇA AQUI: Buscar no map de unique_ptr
					if (auto search = texturesCache.find(tilesetName);
						search != texturesCache.end()) {
						// Usar .get() para pegar o raw pointer
						texture = search->second.get();
					}
					else {
						// Carregar nova textura
						tilesetTexturePath = resPath + TEXTURES_FOLDER_PATH +
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

					// ... resto do código de renderização igual
					SDL_Rect tileRect;
					tileRect.x = tileObject.getDrawingRect().x;
					tileRect.y = tileObject.getDrawingRect().y;
					tileRect.w = tileObject.getDrawingRect().width;
					tileRect.h = tileObject.getDrawingRect().height;

					SDL_Rect renderTile;
					renderTile.x = (x * tileRect.w) - Globals::camera.x;
					renderTile.y = (y * tileRect.h) - Globals::camera.y;
					renderTile.w = tileRect.w;
					renderTile.h = tileRect.h;

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

void Game::checkAndHandleEnemyLoot(Entity* entity) {
	if (entity->type != "enemy") return;

	if (entity->dropItemFlag)  {
		spawnItem(entity->dropItemId, entity->dropItemQty, entity->dropItemXPos, entity->dropItemYPos);
		entity->dropItemFlag = false; // Failsafe
	}

	if (isLivingEntityDead(entity) && !entity->dropEssenceFlag) {
		hero->addEssence(entity->essence);
		entity->dropEssenceFlag = true;
	}
}

void Game::checkAndHandleNearItem(Entity* entity) {
	if (dynamic_cast<Item*>(entity) != nullptr) {
		Item* i = (Item*)entity;
		if (i->isOnGround &&
			(Entity::distanceBetweenTwoPoints(hero->x, hero->y + (hero->collisionBoxYOffset / 2), i->x, i->y) < 40.0)) {
			if (!i->isNearHero) {
				i->isNearHero = true;
				hero->nearItems.push_back(i);
			}
		}
		else {
			i->isNearHero = false;
			hero->nearItems.remove(i);
		}
	}
}

void Game::checkAndHandleNearDoor(Entity* entity) {
	if (dynamic_cast<Door*>(entity) != nullptr) {
		Door* d = (Door*)entity;
		if (d->isClosed &&
			(Entity::distanceBetweenTwoPoints(
				hero->x, hero->y + (hero->collisionBoxYOffset / 2), d->x + 32, d->y) < 60.0)) {
			hero->nearestDoor = d;
			if (!actionMessageUi->isUiLocked()) {
				actionMessageUi->setMessage("Open door");
			}
		}
		else {
			hero->nearestDoor = nullptr;
			actionMessageUi->unlock();

			if (!d->isClosed) {
				if (std::find(openDoorsIds.begin(), openDoorsIds.end(), d->id) == openDoorsIds.end()) {
					openDoorsIds.push_back(d->id);
				}
			}
		}
	}
}

void Game::checkAndHandleNearCheckpoint(Entity* entity) {
	if (dynamic_cast<Checkpoint*>(entity) != nullptr) {
		Checkpoint* cp = (Checkpoint*)entity;
		if (Entity::distanceBetweenTwoPoints(
			hero->x, hero->y + (hero->collisionBoxYOffset / 2.f), cp->x + 32.f, cp->y) < 60.0) {
			hero->nearestCheckpoint = cp;

			cp->isActivated ?
				actionMessageUi->unlock() :
				actionMessageUi->setMessage("Activate checkpoint");
		}
		else {
			hero->nearestCheckpoint = nullptr;
			actionMessageUi->unlock();
		}

		if (hero->isCheckpointActivatedFlag) {
			// saves checkpoint activated status
			hero->isCheckpointActivatedFlag = false;
			saveCheckpointActivatedState(cp->id);
			actionMessageUi->setMessage("Checkpoint active!");
			actionMessageUi->setTimer(3.f);
		}

		if (hero->state == (int)HERO_STATE::RESTING && hero->isRested) {
			hero->isRested = false;
			mustRemoveAllEnemies = true;
			mustSpawnEnemies = true;
		}
	}
}

void Game::checkAndHandleNearBloodstain(Entity* entity) {
	if (dynamic_cast<Bloodstain*>(entity) != nullptr) {
		Bloodstain* bloodstain = (Bloodstain*)entity;
		if (bloodstain->isLive) {
			if (Entity::distanceBetweenTwoPoints(
				hero->x, hero->y + (hero->collisionBoxYOffset / 2.f), bloodstain->x + 32.f, bloodstain->y) < 60.0) {
				hero->nearestBloodstain = bloodstain;
				actionMessageUi->setMessage("Recover lost essence");
			}
			else {
				hero->nearestBloodstain = nullptr;
			}
		}
	}
}

void Game::handleMapChange(bool isHeroRespawn) {
	if (isHeroRespawn) {
		saveGame(true);
		hero->revive();
		loadGame();
		currentMap->file = saveHandler.getCurrentMapFile();
		hero->hp = saveHandler.getHeroHp();
		hero->x = saveHandler.getHeroX();
		hero->y = saveHandler.getHeroY();
	}
	else {
		currentMap->file = currentMap->nextMapWaypoint.nextMapFile;
		hero->x = currentMap->nextMapWaypoint.xDestination;
		hero->y = currentMap->nextMapWaypoint.yDestination;

		fadeIn = false;
		fadeOut = true;
	}

	hero->attackBuffer.clear();
	loadTiledMap(resPath + MAPS_FOLDER_PATH + currentMap->file);

	// Remove enemies
	for (list<Entity*>::iterator enemy = currentMapEnemies.begin(); enemy != currentMapEnemies.end(); enemy++) {
		(*enemy)->active = false;
	}

	// Remove walls, doors and checkpoints
	for (list<Entity*>::iterator entity = Entity::entities.begin(); entity != Entity::entities.end(); entity++) {
		if ((*entity)->type == "wall" ||
				(*entity)->type == "door" ||
				(*entity)->type == "checkpoint" ||
				(*entity)->type == "boss") {
			(*entity)->active = false;
		}
	}

	currentBoss = nullptr;

	mustSpawnEnemies = true;
	hero->currentMap = currentMap;
	spawnItemsFromCurrentMap();
	buildWalls();
	buildWaypoints();
	buildDoors();
	spawnCheckpoints();
	if (bloodstain->isLive &&
		currentMap->file == bloodstain->mapName) {
		bloodstain->create();
	}
	else {
		bloodstain->destroy();
	}
	openDoorsIds = {};
}

void Game::buildDoors() {
	auto tMap = tiledMap.get();
	if (tMap == nullptr) {
		cout << "Mapa nulo" << endl;
		return;
	}

	auto layer = tMap->getLayer("Doors");
	if (layer == nullptr) return;

	int idCounter = 0;
	for (auto& [pos, tileObject] : layer->getTileObjects()) {
		auto animPrefixProp = tileObject.getTile()->getProp("animPrefix");
		auto isLockedProp = tileObject.getTile()->getProp("isLocked");
		auto animSetProp = tileObject.getTile()->getProp("animSet");

		if (animPrefixProp == nullptr ||
			isLockedProp == nullptr ||
			animSetProp == nullptr) continue;

		string animName = std::any_cast<string>(animSetProp->getValue());

		int doorId = 1000 + (currentMap->id * 10) + idCounter;

		if (animName == "doubleDoors") {
			bool isDoorClosed = true;

			if (!openDoorsIds.empty()) {
				for (int id : openDoorsIds) {
					if (id == doorId) isDoorClosed = false;
				}
			}

			Door* door = new Door(
				doubleDoorsAnimSet.get(),
				doorId,
				std::any_cast<string>(animPrefixProp->getValue()),
				isDoorClosed,
				tileObject.getTile()->getPosition(pos).x,
				tileObject.getTile()->getPosition(pos).y,
				64,
				64,
				-32);
			door->isLocked = std::any_cast<bool>(isLockedProp->getValue()),
			Entity::entities.push_back(door);
		}

		idCounter++;
	}
}

void Game::buildWalls() {
	auto currentMap = tiledMap.get();
	if (currentMap == nullptr) {
		cout << "Mapa nulo" << endl;
		return;
	}

	for (auto layer : currentMap->getLayers()) {
		if (&layer == nullptr) return;

		for (auto& [pos, tileObject] : layer.getTileObjects()) {
			for (auto it : tileObject.getTile()->getObjectgroup().getObjectsByName("Wall")) {
				Wall* newWall = new Wall(it.getSize().x, it.getSize().y, 0);
				newWall->x = tileObject.getPosition().x + 16;
				newWall->y = tileObject.getPosition().y;
				walls.push_back(newWall);
				Entity::entities.push_back(newWall);
			}
		}
	}
}

void Game::buildWaypoints() {
	auto tMap = tiledMap.get();
	if (tMap == nullptr) {
		cout << "Mapa nulo" << endl;
		return;
	}

	auto layer = tMap->getLayer("Waypoints");
	for (auto object : layer->getObjects()) {
		Map::Waypoint waypoint;
		waypoint.nextMapFile = std::any_cast<string>(object.getProp("nextMapFileName")->getValue());
		waypoint.xDestination = std::any_cast<int>(object.getProp("xDest")->getValue());
		waypoint.yDestination = std::any_cast<int>(object.getProp("yDest")->getValue());
		waypoint.waypointRect.x = object.getPosition().x;
		waypoint.waypointRect.y = object.getPosition().y;
		waypoint.waypointRect.w = object.getSize().x;
		waypoint.waypointRect.h = object.getSize().y;

		currentMap->currentMapWaypoints.push_back(waypoint);

		if (isBossMap()) {
			Wall* fogWall = new Wall(
				waypoint.waypointRect.w + 2,
				waypoint.waypointRect.h + 2,
				0);

			fogWall->x = waypoint.waypointRect.x - 1;
			fogWall->y = waypoint.waypointRect.y - 1;
			fogWalls.push_back(fogWall);
			Entity::entities.push_back(fogWall);

		}
	}
}

void Game::draw() {
	// clear screen
	SDL_SetRenderDrawColor(Globals::renderer, 145, 133, 129, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(Globals::renderer);

	if (splashShowing) {
		renderTexture(splashImage, Globals::renderer, 0, 0);
	}
	else {

		updateMaps();

		// sort all entities based on y (depth)
		Entity::entities.sort(Entity::EntityCompare);

		// draw all of the entities
		for (list<Entity*>::iterator entity = Entity::entities.begin(); entity != Entity::entities.end(); entity++) {
			(*entity)->draw();
		}

		// draw all of the UI
		for (list<Ui*>::iterator ui = gui.begin(); ui != gui.end(); ui++) {
			(*ui)->draw();
		}

		if (overlayTimer <= 0 && hero->hp < 1) {
			renderTexture(overlayImage, Globals::renderer, 0, 0);

			if (scoreTexture == NULL) {
				//generate score text
				SDL_Color color = { 255, 255, 255, 255 };

				stringstream ss;
				ss << "Enemies dispatched: " << Glob::globsKilled + Grob::grobsKilled + RoundKing::roundKingsKilled;

				scoreTexture = renderText(ss.str(), resPath + Ui::FONTS_PATH + "vermin_vibes_1989.ttf", color, 30, Globals::renderer);
			}

			renderTexture(scoreTexture, Globals::renderer, 20, 50);
		}
	}

	// after done drawing, show it to the screen
	SDL_RenderPresent(Globals::renderer);
}

void Game::spawnEnemies() {
	mustSpawnEnemies = false;

	auto tMap = tiledMap.get();
	if (tMap == nullptr) {
		cout << "Mapa nulo" << endl;
		return;
	}

	auto layer = tMap->getLayer("EnemiesSpawn");
	if (layer == nullptr) {
		return;
	}

	int enemyPosX, enemyPosY;
	int uniqueId;
	int idCounter = 0;
	for (auto object : layer->getObjects()) {
		int enemyId = std::any_cast<int>(object.getProp("enemyId")->getValue());
		uniqueId = (enemyId * 10) + idCounter;
		enemyPosX = object.getPosition().x;
		enemyPosY = object.getPosition().y;
		switch (enemyId) {
		case 0: // Glob
			if (deadEnemiesIds.empty() ||
				std::find(deadEnemiesIds.begin(), deadEnemiesIds.end(), uniqueId) == deadEnemiesIds.end()) {
				TermiteMiner* enemy = new TermiteMiner(termiteMinerAnimSet.get());
					enemy->x = enemyPosX;
					enemy->y = enemyPosY;
					enemy->invincibleTimer = 0.1;
					enemy->enemyId = uniqueId;
					currentMapEnemies.push_back(enemy);
					Entity::entities.push_back(enemy);
			}
			break;
		case 1: // Termite
			if (deadEnemiesIds.empty() ||
				std::find(deadEnemiesIds.begin(), deadEnemiesIds.end(), uniqueId) == deadEnemiesIds.end()) {
					TermiteMiner* enemy = new TermiteMiner(termiteMinerAnimSet.get());
					enemy->x = enemyPosX;
					enemy->y = enemyPosY;
					enemy->invincibleTimer = 0.1;
					enemy->enemyId = uniqueId;
					currentMapEnemies.push_back(enemy);
					Entity::entities.push_back(enemy);
			}
			break;
		default:
			break;
			
		}

		idCounter++;
	}
}

void Game::spawnBoss() {
	auto tMap = tiledMap.get();
	if (tMap == nullptr) {
		cout << "Mapa nulo" << endl;
		return;
	}

	auto layer = tMap->getLayer("BossSpawn");
	if (layer == nullptr) return;

	for (auto object : layer->getObjects()) {
		int bossId = std::any_cast<int>(object.getProp("bossId")->getValue());
		for (int i : defeatedBossesIds) {
			if (i == bossId) return;
		}

		int bossPosX = object.getPosition().x;
		int bossPosY = object.getPosition().y;
		switch (bossId) {
			case 990001: // Small Brown Spider
				currentBoss = new SmallBrownSpider(smallBrownSpiderAnimSet.get());
				currentBoss->x = bossPosX;
				currentBoss->y = bossPosY;
				currentBoss->id = bossId;
				currentBoss->invincibleTimer = 0.1;
				//currentMapEnemies.push_back(currentBoss);
				Entity::entities.push_back(currentBoss);
				bossHpBar = new HPBar(currentBoss, BarType::BOSS_HEALTH_BAR); // Exemplo
				gui.push_back(bossHpBar);
				break;
			default:
				break;
		}
	}
}

void Game::spawnItem(int itemId, int quant, int xPos, int yPos) {
	//TODO: Spawnar itens usando a lista currentMap->itemsInMap
	//TODO: Criar switch case com enums do itemId
	Item* spawnItem;
	bool canSpawn = true;

	switch (itemId) {
	case Item::HONEYDEW_POTION_ID:
		spawnItem = new HoneydewPotion(hDewPotionAnimSet.get(), canSpawn, quant);
		break;
	case Item::GREEN_BERRY_ID:
		spawnItem = new GreenBerry(hDewPotionAnimSet.get(), canSpawn, quant);
		break;
	case Item::STONE_ID:
		spawnItem = new Stone(hDewPotionAnimSet.get(), canSpawn, quant);
		spawnItem->projectileAnimSet = stoneProjectileAnimSet.get();
		break;
	default:
		return;
	}
	
	spawnItem->x = xPos;
	spawnItem->y = yPos;
	spawnItem->active = true;
	Entity::entities.push_back(spawnItem);
}

void Game::spawnCheckpoints() {
	auto tMap = tiledMap.get();
	if (tMap == nullptr) {
		cout << "Mapa nulo" << endl;
		return;
	}

	auto layer = tMap->getLayer("Checkpoints");
	if (layer == nullptr) return;

	for (auto object : layer->getObjects()) {
		bool isActive = std::any_cast<bool>(object.getProp("isActive")->getValue());
		int cpId = std::any_cast<int>(object.getProp("checkpointId")->getValue());
		int cpPosX = object.getPosition().x;
		int cpPosY = object.getPosition().y;

		Checkpoint* checkpoint = new Checkpoint(checkpointAnimSet.get(), cpId, currentMap->file);
		checkpoint->x = cpPosX;
		checkpoint->y = cpPosY;
		if (isActive) checkpoint->activate();

		Entity::entities.push_back(checkpoint);
	}
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
		if (!currentBoss->dropEssenceFlag) {
			hero->addEssence(currentBoss->essence);
			currentBoss->dropEssenceFlag = true;
		}		
		// currentBoss = nullptr;
	}
}

void Game::saveCheckpointActivatedState(int checkpointId) {
	const string mapFilePath = getResourcePath() + MAPS_FOLDER_PATH + currentMap->file;
	std::ifstream ifs(mapFilePath);
	json mapFile = json::parse(ifs);
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
								std::ofstream outputFile(mapFilePath);
								prop2["value"].clear();
								prop2["value"] = true;
								outputFile << std::setw(4) << mapFile << std::endl;
								outputFile.close();
							}
						}
					}
				}
			}
		}
	}
}

void Game::loadAnimationSets() {
	list<DataGroupType> dataGroupTypes; //describes the types of groups the data can have

	// what data can the frame have?
	// collisionBoxes
	DataGroupType colBoxType;
	colBoxType.groupName = "collisionBox";
	colBoxType.dataType = DataGroupType::DATATYPE_BOX;

	// hitboxes
	DataGroupType hitBoxType;
	hitBoxType.groupName = "hitBox";
	hitBoxType.dataType = DataGroupType::DATATYPE_BOX;

	// damage
	DataGroupType dmgType;
	dmgType.groupName = "damage";
	dmgType.dataType = DataGroupType::DATATYPE_NUMBER;

	// add all of these to the list
	dataGroupTypes.push_back(colBoxType);
	dataGroupTypes.push_back(hitBoxType);
	dataGroupTypes.push_back(dmgType);

	hDewPotionAnimSet = std::make_unique<AnimationSet>();
	hDewPotionAnimSet->loadAnimationSet(
		ANIMATIONS_FOLDER_PATH + "groundConsumableItem.fdset", dataGroupTypes);

	heroAnimSet = std::make_unique<AnimationSet>();
	heroAnimSet->loadAnimationSet(
		ANIMATIONS_FOLDER_PATH + "antHero.fdset", dataGroupTypes, true, 0, true);

	globAnimSet = std::make_unique<AnimationSet>();
	globAnimSet->loadAnimationSet(
		ANIMATIONS_FOLDER_PATH + "glob.fdset", dataGroupTypes, true, 0, true);

	grobAnimSet = std::make_unique<AnimationSet>();
	grobAnimSet->loadAnimationSet(
		ANIMATIONS_FOLDER_PATH + "grob.fdset", dataGroupTypes, true, 0, true);

	termiteMinerAnimSet = std::make_unique<AnimationSet>();
	termiteMinerAnimSet->loadAnimationSet(
		ANIMATIONS_FOLDER_PATH + "termiteMiner.fdset", dataGroupTypes, true, 0, true);

	wallAnimSet = std::make_unique<AnimationSet>();
	wallAnimSet->loadAnimationSet(
		ANIMATIONS_FOLDER_PATH + "wall.fdset", dataGroupTypes);

	roundKingAnimSet = std::make_unique<AnimationSet>();
	roundKingAnimSet->loadAnimationSet(
		ANIMATIONS_FOLDER_PATH + "roundKing.fdset", dataGroupTypes, true, 0, true);

	smallBrownSpiderAnimSet = std::make_unique<AnimationSet>();
	smallBrownSpiderAnimSet->loadAnimationSet(
		ANIMATIONS_FOLDER_PATH + "spider_boss.fdset", dataGroupTypes);

	bulletAnimSet = std::make_unique<AnimationSet>();
	bulletAnimSet->loadAnimationSet(
		ANIMATIONS_FOLDER_PATH + "bullet.fdset", dataGroupTypes, true, 0, true);

	stoneProjectileAnimSet = std::make_unique<AnimationSet>();
	stoneProjectileAnimSet->loadAnimationSet(
		ANIMATIONS_FOLDER_PATH + "stoneProjectile.fdset", dataGroupTypes, true, 0, true);

	doubleDoorsAnimSet = std::make_unique<AnimationSet>();
	doubleDoorsAnimSet->loadAnimationSet(
		ANIMATIONS_FOLDER_PATH + "double_doors.fdset", dataGroupTypes);

	checkpointAnimSet = std::make_unique<AnimationSet>();
	checkpointAnimSet->loadAnimationSet(
		ANIMATIONS_FOLDER_PATH + "checkpoint.fdset", dataGroupTypes);

	bloodstainAnimSet = std::make_unique<AnimationSet>();
	bloodstainAnimSet->loadAnimationSet(
		ANIMATIONS_FOLDER_PATH + "bloodstain.fdset", dataGroupTypes);
}

void Game::spawnItemsFromCurrentMap() {
	auto currentTiledMap = tiledMap.get();
	if (currentTiledMap == nullptr) {
		cout << "Mapa nulo" << endl;
		return;
	}
	for (auto layer : currentTiledMap->getLayers()) {
		if (&layer == nullptr) return;

		if (layer.getType() == tson::LayerType::ObjectGroup &&
			layer.getName() == "Items") {
			bool isPicked;
			int itemId;
			int xPos;
			int yPos;
			for (auto object : layer.getObjects()) {
				for (auto prop : object.getProperties().getProperties()) {
					if (prop.second.getName() == "isPicked") {
						isPicked = std::any_cast<bool>(prop.second.getValue());
					}
					else if (prop.second.getName() == "itemId") {
						itemId = std::any_cast<int>(prop.second.getValue());
					}
				}
				xPos = object.getPosition().x;
				yPos = object.getPosition().y;
				currentMap->itemsInMap.push_back(
					std::make_pair(isPicked, std::make_tuple(itemId, xPos, yPos)));

				if (!isPicked) {
					spawnItem(itemId, 1, xPos, yPos);
				}
			}
		}
	}
}

void Game::inactivateCurrentMapItems() {
	for (list<Entity*>::iterator entity = Entity::entities.begin(); entity != Entity::entities.end(); entity++) {
		if (dynamic_cast<Item*>((*entity)) != nullptr) {
			Item* i = (Item*)(*entity);
			i->active = false;
		}
	}
}

void Game::removeAllEnemiesInMap() {
	for (list<Entity*>::iterator enemy = currentMapEnemies.begin(); enemy != currentMapEnemies.end(); enemy++) {
		(*enemy)->active = false;
	}
	Entity::removeInactiveEntitiesFromList(&Entity::entities, false);
	Entity::removeInactiveEntitiesFromList(&currentMapEnemies, true);
	currentMapEnemies.clear();
	deadEnemiesIds.clear();
}

map<int, std::unique_ptr<Item>> Game::loadInventoryItems(
	std::vector<std::pair<int, int>> items) {

	map<int, std::unique_ptr<Item>> loadedItems;

	if (!items.empty()) {
		for (auto item : items) {
			std::unique_ptr<Item> loadItem;

			switch (item.first) {
			case Item::HONEYDEW_POTION_ID:
				loadItem = std::make_unique<HoneydewPotion>(
					hDewPotionAnimSet.get(), false, item.second);
				loadItem->active = false;
				break;
			case Item::GREEN_BERRY_ID:
				loadItem = std::make_unique<GreenBerry>(
					hDewPotionAnimSet.get(), false, item.second);
				loadItem->active = false;
				break;
			case Item::STONE_ID:
				loadItem = std::make_unique<Stone>(
					hDewPotionAnimSet.get(), false, item.second);
				loadItem->projectileAnimSet = stoneProjectileAnimSet.get();
				loadItem->active = false;
				break;
			case Item::COMMON_KEY_ID:
				loadItem = std::make_unique<Key>(
					hDewPotionAnimSet.get(), false, item.second);
				loadItem->active = false;
				break;
			default:
				cout << "Item " << item.first << " nao mapeado\n";
				continue;
			}

			if (loadItem != nullptr) {
				int itemId = loadItem->id;
				loadedItems.emplace(itemId, std::move(loadItem));
			}
		}
	}

	return loadedItems;
}

void Game::saveGame(bool isCheckpointSave) {
	cout << "Saving..." << endl;

	float hp;
	int x, y;
	string mapFile;

	if (isCheckpointSave) {
		hp = hero->hpMax;
		x = hero->lastCheckpointPos.x;
		y = hero->lastCheckpointPos.y;
		mapFile = hero->lastCheckpointMapFile;
	}
	else {
		hp = hero->hp;
		x = hero->x;
		y = hero->y;
		mapFile = currentMap->file;
	}

	std::vector<std::pair<int, int>> inventory;
	for (auto& [id, item] : hero->inventory) {
		inventory.push_back(std::make_pair(id, item->quantity));
	}

	saveHandler.save(
		hp,
		x,
		y,
		hero->essence,
		mapFile,
		inventory,
		openDoorsIds,
		defeatedBossesIds,
		bloodstain->x,
		bloodstain->y,
		bloodstain->essence,
		bloodstain->mapName);
}

void Game::loadGame() {
	//TODO: Trazer rotina de carregamento
	if (!saveHandler.load()) {
		cerr << "Erro ao carregar savefile" << endl;
		terminate();
	}
}

GameState Game::getGameState() {
	return gameState;
}
