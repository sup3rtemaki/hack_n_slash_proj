#include "game.h"

#include "door.h"

#include <iostream>
#include <fstream>
#include <string>

#include "tileson/tileson.hpp"
#include "nlohmann/json.hpp"
#include "item/key.h"
#include "npcs/bosses/smallBrownSpider.h"

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

	splashShowing = true;
	overlayTimer = 2;

	//setup camera
	Globals::camera.x = 0;
	Globals::camera.y = 0;
	Globals::camera.w = Globals::ScreenWidth;
	Globals::camera.h = Globals::ScreenHeight;

	//loadup sounds
	SoundManager::soundManager.loadSound(SOUNDS_FOLDER_PATH + "hit", resPath + "Randomize2.wav");
	SoundManager::soundManager.loadSound(SOUNDS_FOLDER_PATH + "enemyHit", resPath + "Hit_Hurt9.wav");
	SoundManager::soundManager.loadSound(SOUNDS_FOLDER_PATH + "swing", resPath + "Randomize21.wav");
	SoundManager::soundManager.loadSound(SOUNDS_FOLDER_PATH + "dash", resPath + "dash.wav");
	SoundManager::soundManager.loadSound(SOUNDS_FOLDER_PATH + "growl", resPath + "Randomize34.wav");
	SoundManager::soundManager.loadSound(SOUNDS_FOLDER_PATH + "enemyDie", resPath + "Randomize41.wav");
	SoundManager::soundManager.loadSound(SOUNDS_FOLDER_PATH + "crash", resPath + "crash.wav");
	SoundManager::soundManager.loadSound(SOUNDS_FOLDER_PATH + "smash", resPath + "smash.wav");
	SoundManager::soundManager.loadSound(SOUNDS_FOLDER_PATH + "shoot", resPath + "shoot2.wav");
	SoundManager::soundManager.loadSound(SOUNDS_FOLDER_PATH + "laugh", resPath + "laugh2.wav");

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
	hero = new Hero(heroAnimSet);
	hero->invincibleTimer = 0;
	hero->hp = saveHandler.getHeroHp();
	hero->x = saveHandler.getHeroX();
	hero->y = saveHandler.getHeroY();
	hero->inventory.clear();
	hero->inventory = loadInventoryItems(saveHandler.getItems());
	for (auto i : hero->inventory) {
		hero->addItemToQuickAccess(i.first);
		hero->quickAccessInventoryIndex++;
	}
	hero->quickAccessInventoryIndex = 0;

	openDoorsIds = saveHandler.getOpenDoorsIds();

	heroKeyboardInput.hero = hero;
	heroJoystickInput.hero = hero;
	hero->currentMap = currentMap;
	Entity::entities.push_back(hero);

	buildWalls();
	buildWaypoints();
	buildDoors();

	spawnItemsFromCurrentMap();

	//get camera to follow hero
	camController.target = hero;

	quickItemUi = new QuickItemUi(hero);
	itemPickMessageUi = new ItemPickMessageUi(hero);
	actionMessageUi = new ActionMessageUi();
	heroHpBar = new HPBar(hero, BarType::HERO_HEALTH_BAR);
	heroStBar = new HPBar(hero, BarType::HERO_STAMINA_BAR);
	hero->actionMessageUi = actionMessageUi;

	gui.push_back(quickItemUi);
	gui.push_back(itemPickMessageUi);
	gui.push_back(actionMessageUi);
	gui.push_back(heroHpBar);
	gui.push_back(heroStBar);

	buildBossNext = false;
	bossActive = false;

	camController.isLerping = true;

	updateMaps();

	// teste
	SmallBrownSpider* enemy = new SmallBrownSpider(smallBrownSpiderAnimSet);
	enemy->x = hero->x + 32;
	enemy->y = hero->y + 32;
	enemy->invincibleTimer = 0.1;
	//currentMapEnemies.push_back(enemy);
	Entity::entities.push_back(enemy);
}

Game::~Game() {
	cleanup(splashImage);
	cleanup(overlayImage);
	cleanup(fadeImage);

	if (scoreTexture != NULL) {
		cleanup(scoreTexture);
	}

	texturesCache.clear();

	Mix_PausedMusic();
	Mix_FreeMusic(song);

	Entity::removeAllFromList(&Entity::entities, false);
	delete heroAnimSet;
	delete globAnimSet;
	delete grobAnimSet;
	delete termiteMinerAnimSet;
	delete wallAnimSet;
	delete roundKingAnimSet;
	delete bulletAnimSet;
	delete stoneProjectileAnimSet;
	delete hero;

	Entity::removeAllFromList(&walls, true);
	Entity::removeAllFromList(&currentMapEnemies, true);
	deadEnemiesIds.clear();
}

void Game::update() {
	int enemiesBuilt = 0;
	float enemyBuildTimer = 1;
	bool quit = false;
	SDL_Event e;
	
	// setup time controller before game starts
	TimeController::timeController.reset();

	//game loop
	while (!quit) {
		TimeController::timeController.updateTime();

		Entity::removeInactiveEntitiesFromList(&Entity::entities, false);

		LivingEntity::saveDeadEnemiesIds(currentMapEnemies, deadEnemiesIds);

		//remove and delete enemies in the list who are dead/inactive
		Entity::removeInactiveEntitiesFromList(&currentMapEnemies, true);
		
		// check for any events that might have happened
		while (SDL_PollEvent(&e)) {
			// close the window
			if (e.type == SDL_QUIT) {
				quit = true;
			}

			// keydown event
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.scancode) {
				case SDL_SCANCODE_ESCAPE:
					quit = true;
					break;
				case SDL_SCANCODE_SPACE:
					if (splashShowing) {
						splashShowing = false;
					}

					if (overlayTimer <= 0 && hero->hp < 1) {
						//cleanup and restart game
						enemiesBuilt = 0;
						enemyBuildTimer = 3;
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

						mustSpawnEnemies = true;
						hero->revive();
					}
					
					break;
				}
			}
			if (!isFading) {
				heroKeyboardInput.update(&e);
				heroJoystickInput.update(&e);
			}
			else {
				hero->moving = false;
			}
		}

		//make overlay timer tick down
		if (hero->hp < 1 && overlayTimer > 0) {
			overlayTimer -= TimeController::timeController.dT;
		}

		// update all entites
		for (list<Entity*>::iterator entity = Entity::entities.begin(); entity != Entity::entities.end(); entity++) {
			// update all entites in world at once (polymorphism)
			(*entity)->update();

			// checks if entity is an item, and if we are close to it
			if (dynamic_cast<Item*>((*entity)) != nullptr) {
				Item* i = (Item*)(*entity);
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

			if ((*entity)->type == "enemy" && (*entity)->dropItemFlag) {
				spawnItem((*entity)->dropItemId, (*entity)->dropItemQty, (*entity)->dropItemXPos, (*entity)->dropItemYPos);
				(*entity)->dropItemFlag = false; // Failsafe
			}

			if (dynamic_cast<Door*>((*entity)) != nullptr) {
				Door* d = (Door*)(*entity);
				if (d->isClosed &&
					(Entity::distanceBetweenTwoPoints(hero->x, hero->y + (hero->collisionBoxYOffset / 2), d->x + 32, d->y) < 60.0)) {
					// Teste
					hero->nearestDoor = d;
					if (!actionMessageUi->isUiLocked()) {
						actionMessageUi->setMessage("Open door");
					}
				}
				else {
					hero->nearestDoor = nullptr;
					actionMessageUi->unlock();

					if (!d->isClosed) {
						if(std::find(openDoorsIds.begin(), openDoorsIds.end(), d->id) == openDoorsIds.end()) {
							openDoorsIds.push_back(d->id);
						}
					}
				}
			}
		}

		//spawn enemies
		if (hero->hp > 0 && !splashShowing) {
			if (currentMapEnemies.size() <= 0 && mustSpawnEnemies) {
				spawnEnemies();
			}
			/*if (enemiesToBuild == enemiesBuilt && currentMapEnemies.size() <= 0) {
				enemiesToBuild = enemiesToBuild + 6;
				enemiesBuilt = 0;
				enemyBuildTimer = 4;
				enemyWavesTillBoss--;

				if (enemyWavesTillBoss <= 0) {
					buildBossNext = true;
				}
			}*/

			//enemyBuildTimer -= TimeController::timeController.dT;
			enemyBuildTimer = 0;

			//boss
			if (buildBossNext && enemyBuildTimer <= 0 && currentMapEnemies.size() == 0) {
				RoundKing* round = new RoundKing(roundKingAnimSet, bulletAnimSet);
				round->invincibleTimer = 0.1;
				currentMapEnemies.push_back(round);
				Entity::entities.push_back(round);

				//make hpbar point to boss
				bossHpBar = new HPBar(round, BarType::BOSS_HEALTH_BAR); // Exemplo
				bossActive = true;
				buildBossNext = false;
				enemyWavesTillBoss = 3;
			}

			//reset spawn waves
			if (bossActive && currentMapEnemies.size() == 0) {
				bossActive = false;
				buildBossNext = false;
				enemiesBuilt = 0;
				bossHpBar->entity = nullptr; // when boss dies, hpbar doesnt reference him anymore
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

		// draw all entites
		draw();

		// update camera position
		camController.update();

		// framerate
		// cout << TimeController::timeController.dT << endl;
	}
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
														std::ofstream outputFile(getResourcePath() + currentMap->file);
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

				const string& resPath = getResourcePath();

				currentMap->file = currentMap->nextMapWaypoint.nextMapFile;
				loadTiledMap(resPath + MAPS_FOLDER_PATH + currentMap->file);
				hero->x = currentMap->nextMapWaypoint.xDestination;
				hero->y = currentMap->nextMapWaypoint.yDestination;

				fadeIn = false;
				fadeOut = true;

				// Remove enemies
				for (list<Entity*>::iterator enemy = currentMapEnemies.begin(); enemy != currentMapEnemies.end(); enemy++) {
					(*enemy)->active = false;
				}

				// Remove walls and doors
				for (list<Entity*>::iterator entity = Entity::entities.begin(); entity != Entity::entities.end(); entity++) {
					if ((*entity)->type == "wall" || (*entity)->type == "door") {
						(*entity)->active = false;
					}
				}

				mustSpawnEnemies = true;
				hero->currentMap = currentMap;
				spawnItemsFromCurrentMap();
				buildWalls();
				buildWaypoints();
				buildDoors();
				saveGame();
				openDoorsIds = {};
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

	int mapId = std::any_cast<int>(currentMap->getProp("id")->getValue());
	if (mapId > 0 && mapId != this->currentMap->id) {
		this->currentMap->id = mapId;
	}

	string tilesetName;
	string tilesetTexturePath;
	int x = 0;
	int y = 0;
	SDL_Texture* texture = nullptr;
	for (auto layer : currentMap->getLayers()) {
		if (&layer == nullptr) {
			cout << "layer null" << endl;
		}

		for (auto& [pos, tileObject] : layer.getTileObjects()) { //Loops through absolutely all existing tiles
			if (tileObject.getTile() == nullptr) {
				cout << "tile null" << endl;
				return;
			}

			if (layer.getType() == tson::LayerType::TileLayer) {
				//Set sprite data to draw the tile
				tson::Tileset* tileset = tileObject.getTile()->getTileset();
				bool hasAnimation = tileObject.getTile()->getAnimation().any();
				tson::Rect drawingRect;


				// Only render the tile if its not marked as transparent
				// Some empty tiles are marked as transparent to fill the layer, but they doesnt
				// need to be drawn
				bool isTransparentTile = std::any_cast<bool>(tileObject.getTile()->getProp("isTransparent")->getValue());
				if (!isTransparentTile) {

					if (!hasAnimation) {
						drawingRect = tileObject.getDrawingRect();
					}

					tilesetName = tileset->getImage().filename().string();
					if (auto search = texturesCache.find(tilesetName); search != texturesCache.end()) {
						texture = search->second;
					}
					else {
						tilesetTexturePath = resPath + TEXTURES_FOLDER_PATH + tileset->getImage().filename().string();
						texture = loadTexture(tilesetTexturePath, Globals::renderer);
						texturesCache.emplace(tilesetName, texture);
					}

					if (texture == nullptr) {
						cout << "texture null " << tilesetName << endl;
						return;
					}

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

void Game::checkOpenDoors() {

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
				doubleDoorsAnimSet,
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

		// bossHpBar->draw();

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

	int enemyPosX, enemyPosY;
	int uniqueId;
	int idCounter = 0;
	auto layer = tMap->getLayer("EnemiesSpawn");
	if (layer == nullptr) {
		return;
	}

	for (auto object : layer->getObjects()) {
		int enemyId = std::any_cast<int>(object.getProp("enemyId")->getValue());
		uniqueId = (enemyId * 10) + idCounter;
		enemyPosX = object.getPosition().x;
		enemyPosY = object.getPosition().y;
		switch (enemyId) {
		case 0: // Glob
			if (deadEnemiesIds.empty() ||
				std::find(deadEnemiesIds.begin(), deadEnemiesIds.end(), uniqueId) == deadEnemiesIds.end()) {
					Glob* enemy = new Glob(globAnimSet);
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
					TermiteMiner* enemy = new TermiteMiner(termiteMinerAnimSet);
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

void Game::spawnItem(int itemId, int quant, int xPos, int yPos) {
	//TODO: Spawnar itens usando a lista currentMap->itemsInMap
	//TODO: Criar switch case com enums do itemId
	Item* spawnItem;
	bool canSpawn = true;

	switch (itemId) {
	case Item::HONEYDEW_POTION_ID:
		spawnItem = new HoneydewPotion(hDewPotionAnimSet, canSpawn, quant);
		break;
	case Item::GREEN_BERRY_ID:
		spawnItem = new GreenBerry(hDewPotionAnimSet, canSpawn, quant);
		break;
	case Item::STONE_ID:
		spawnItem = new Stone(hDewPotionAnimSet, canSpawn, quant);
		spawnItem->projectileAnimSet = stoneProjectileAnimSet;
		break;
	default:
		return;
	}
	
	spawnItem->x = xPos;
	spawnItem->y = yPos;
	spawnItem->active = true;
	Entity::entities.push_back(spawnItem);
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

	hDewPotionAnimSet = new AnimationSet();
	hDewPotionAnimSet->loadAnimationSet(ANIMATIONS_FOLDER_PATH + "groundConsumableItem.fdset", dataGroupTypes);

	heroAnimSet = new AnimationSet();
	heroAnimSet->loadAnimationSet(ANIMATIONS_FOLDER_PATH + "antHero.fdset", dataGroupTypes, true, 0, true);//"udemyCyborg.fdset", dataGroupTypes, true, 0, true);

	globAnimSet = new AnimationSet();
	globAnimSet->loadAnimationSet(ANIMATIONS_FOLDER_PATH + "glob.fdset", dataGroupTypes, true, 0, true);

	grobAnimSet = new AnimationSet();
	grobAnimSet->loadAnimationSet(ANIMATIONS_FOLDER_PATH + "grob.fdset", dataGroupTypes, true, 0, true);

	termiteMinerAnimSet = new AnimationSet();
	termiteMinerAnimSet->loadAnimationSet(ANIMATIONS_FOLDER_PATH + "termiteMiner.fdset", dataGroupTypes, true, 0, true);

	wallAnimSet = new AnimationSet();
	wallAnimSet->loadAnimationSet(ANIMATIONS_FOLDER_PATH + "wall.fdset", dataGroupTypes);

	roundKingAnimSet = new AnimationSet();
	roundKingAnimSet->loadAnimationSet(ANIMATIONS_FOLDER_PATH + "roundKing.fdset", dataGroupTypes, true, 0, true);

	smallBrownSpiderAnimSet = new AnimationSet();
	smallBrownSpiderAnimSet->loadAnimationSet(ANIMATIONS_FOLDER_PATH + "spider_boss.fdset", dataGroupTypes);

	bulletAnimSet = new AnimationSet();
	bulletAnimSet->loadAnimationSet(ANIMATIONS_FOLDER_PATH + "bullet.fdset", dataGroupTypes, true, 0, true);

	stoneProjectileAnimSet = new AnimationSet();
	stoneProjectileAnimSet->loadAnimationSet(ANIMATIONS_FOLDER_PATH + "stoneProjectile.fdset", dataGroupTypes, true, 0, true);

	doubleDoorsAnimSet = new AnimationSet();
	doubleDoorsAnimSet->loadAnimationSet(ANIMATIONS_FOLDER_PATH + "double_doors.fdset", dataGroupTypes);
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

map<int, Item*> Game::loadInventoryItems(std::vector<std::pair<int, int>> items) {
	map<int, Item*> loadedItems;

	if (!items.empty()) {
		for (auto item : items) {
			Item* loadItem = nullptr;
			switch (item.first) {
			case Item::HONEYDEW_POTION_ID:
				loadItem = new HoneydewPotion(hDewPotionAnimSet, false, item.second);
				loadItem->active = false;
				break;
			case Item::GREEN_BERRY_ID:
				loadItem = new GreenBerry(hDewPotionAnimSet, false, item.second);
				loadItem->active = false;
				break;
			case Item::STONE_ID:
				loadItem = new Stone(hDewPotionAnimSet, false, item.second);
				loadItem->projectileAnimSet = stoneProjectileAnimSet;
				loadItem->active = false;
				break;
			case Item::COMMON_KEY_ID:
				loadItem = new Key(hDewPotionAnimSet, false, item.second);
				loadItem->active = false;
				break;
			default:
				cout << "Item " << item.first << " nao mapeado\n";
				break;
			}

			if (loadItem != nullptr) {
				loadedItems.emplace(std::make_pair(loadItem->id, loadItem));
			}
		}
	}
	else {
		cout << "Vetor de items vazio!\n";
	}
	

	return loadedItems;
}

void Game::saveGame() {
	std::vector<std::pair<int, int>> inventory;
	for (auto itemMap : hero->inventory) {
		inventory.push_back(std::make_pair(itemMap.first, itemMap.second->quantity));
	}
	// saveHandler.save(hero->hp, hero->x, hero->y, currentMap->file, inventory, openDoorsIds);
}

void Game::loadGame() {
	//TODO: Trazer rotina de carregamento
	if (!saveHandler.load()) {
		cerr << "Erro ao carregar savefile" << endl;
		terminate();
	}
}
