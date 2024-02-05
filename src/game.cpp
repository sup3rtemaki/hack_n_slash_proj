#include "game.h"

#include <iostream>
#include <fstream>
#include <string>

#include "tileson/tileson.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

tson::Tileson tileson; // Tileson global instance
std::unique_ptr<tson::Map> tiledMap; // Tiled map
std::map<std::tuple<int, int>, tson::Tile*> tileData;

Game::Game() {
	//TODO: Criar m�todo initialize ou algo do tipo pra encapsular tudo isso

	string resPath = getResourcePath();

	//teste
	backGroundImage = loadTexture(resPath + "map.png", Globals::renderer);
	fadeImage = loadTexture(resPath + "blackBG.png", Globals::renderer);

	currentMapId = 0;
	mapToDrawCount = 0;

	// JSON
	std::ifstream ifs(resPath + "mapsPositions.json");
	
	json mapsPositions = json::parse(ifs);

	mapQty = mapsPositions["maps"]["map_count"];

	if (mapQty > 0) {
		for (auto& mapsIt : mapsPositions["maps"]["map"]) {
			string file = mapsIt["file"];
			int id = mapsIt["id"];
			int l_x1 = mapsIt["l_x1"];
			int l_y1 = mapsIt["l_y1"];
			int l_x2 = mapsIt["l_x2"];
			int l_y2 = mapsIt["l_y2"];
			int r_x1 = mapsIt["r_x1"];
			int r_y1 = mapsIt["r_y1"];
			int r_x2 = mapsIt["r_x2"];
			int r_y2 = mapsIt["r_y2"];
			int t_x1 = mapsIt["t_x1"];
			int t_y1 = mapsIt["t_y1"];
			int t_x2 = mapsIt["t_x2"];
			int t_y2 = mapsIt["t_y2"];
			int b_x1 = mapsIt["b_x1"];
			int b_y1 = mapsIt["b_y1"];
			int b_x2 = mapsIt["b_x2"];
			int b_y2 = mapsIt["b_y2"];
			int l_map = mapsIt["l_map"];
			int r_map = mapsIt["r_map"];
			int t_map = mapsIt["t_map"];
			int b_map = mapsIt["b_map"];
			int qt_enemies = mapsIt["qt_enemies"];
			int qt_items = mapsIt["qt_items"];

			cout << "entrou 1" << endl;

			std::vector<std::tuple<int, int, int>> mapEnemies;
			if (qt_enemies > 0) {
				for (auto& enemiesIt : mapsIt["enemy"]) {
					int e_id = enemiesIt["e_id"];
					int e_x = enemiesIt["e_x"];
					int e_y = enemiesIt["e_y"];
					cout << "entrou 2" << endl;
					mapEnemies.push_back(std::make_tuple(e_id, e_x, e_y));
				}
			}
			
			vector<pair<bool, tuple<int, int, int>>> itemsInMapVector;
			for (auto& itemsIt : mapsIt["item"]) {
				int isPickedUpInt = itemsIt["is_picked"];
				bool isPickedUp = (isPickedUpInt == 0) ? false : true;
				cout << "entrou 3" << endl;
				int itemId = itemsIt["item_id"];
				int itemX = itemsIt["item_x"];
				int itemY = itemsIt["item_y"];
				cout << "entrou 3" << endl;
				itemsInMapVector.push_back(
					std::make_pair(isPickedUp, std::make_tuple(itemId, itemX, itemY))
				);
			}

			Map m = Map(id, file, l_x1, l_y1, l_x2, l_y2, r_x1, r_y1, r_x2, r_y2, t_x1, t_y1, t_x2, t_y2, b_x1, b_y1, b_x2, b_y2, l_map, r_map, t_map, b_map, qt_enemies, mapEnemies);
			m.itemsInMap = itemsInMapVector;
			mapList.push_back(m);

			cout << m.id << " " << m.file << " " <<
				m.leftX1 << " " << m.leftY1 << " " << m.leftX2 << " " << m.leftY2 << " " <<
				m.rightX1 << " " << m.rightY1 << " " << m.rightX2 << " " << m.rightY2 << " " <<
				m.topX1 << " " << m.topY1 << " " << m.topX2 << " " << m.topY2 << " " <<
				m.bottomX1 << " " << m.bottomY1 << " " << m.bottomX2 << " " << m.bottomY2 << " " <<
				m.leftMapId << " " << m.rightMapId << " " << m.topMapId << " " << m.bottomMapId << " " <<
				"\n";
		}
	}

	currentMap = &(*std::next(mapList.begin(), currentMapId));

	//Pre-load current and surroundings maps images
	loadTiledMap(resPath + currentMap->file);
	mustSpawnEnemies = true;

	splashImage = loadTexture(resPath + "cyborgtitle.png", Globals::renderer);
	overlayImage = loadTexture(resPath + "overlay.png", Globals::renderer);

	splashShowing = true;
	overlayTimer = 2;

	//setup camera
	Globals::camera.x = 0;
	Globals::camera.y = 0;
	Globals::camera.w = Globals::ScreenWidth;
	Globals::camera.h = Globals::ScreenHeight;

	//loadup sounds
	SoundManager::soundManager.loadSound("hit", resPath + "Randomize2.wav");
	SoundManager::soundManager.loadSound("enemyHit", resPath + "Hit_Hurt9.wav");
	SoundManager::soundManager.loadSound("swing", resPath + "Randomize21.wav");
	SoundManager::soundManager.loadSound("dash", resPath + "dash.wav");
	SoundManager::soundManager.loadSound("growl", resPath + "Randomize34.wav");
	SoundManager::soundManager.loadSound("enemyDie", resPath + "Randomize41.wav");
	SoundManager::soundManager.loadSound("crash", resPath + "crash.wav");
	SoundManager::soundManager.loadSound("smash", resPath + "smash.wav");
	SoundManager::soundManager.loadSound("shoot", resPath + "shoot2.wav");
	SoundManager::soundManager.loadSound("laugh", resPath + "laugh2.wav");

	//song = Mix_LoadMUS(string(resPath + "Fatal Theory.wav").c_str());
	//if (song != NULL) {
	//	Mix_PlayMusic(song, -1);
	//	Mix_Volume(-1, 50);
	//}

	loadAnimationSets();

	saveHandler.load();

	// build hero entity
	hero = new Hero(heroAnimSet);
	hero->invincibleTimer = 0;
	//hero->x = Globals::ScreenWidth / 2;
	//hero->y = Globals::ScreenHeight / 2;
	//TODO: Levar essa rotina pro loadGame
	hero->hp = saveHandler.getHeroHp();
	hero->x = saveHandler.getHeroX();
	hero->y = saveHandler.getHeroY();
	hero->inventory.clear();
	hero->inventory = loadInventoryItems(saveHandler.getItems());
	for (auto i : hero->inventory) {
		hero->addItemToQuickAccess(i.first);
		hero->inventoryIndex++;
	}
	hero->inventoryIndex = 0;

	heroKeyboardInput.hero = hero;
	heroJoystickInput.hero = hero;
	heroHpBar.entity = hero;
	hero->currentMap = currentMap;
	Entity::entities.push_back(hero);

	//TODO: Fazer m�todo para spawnar itens de acordo com mapa
	//spawnItem(Item::HONEYDEW_POTION_ID, 5, 200, 200);
	spawnItemsFromCurrentMap();

	//get camera to follow hero
	camController.target = hero;

	quickItemUi = new QuickItemUi(hero);
	itemPickMessageUi = new ItemPickMessageUi(hero);

	// build the outer walls
	int tileSize = 32;

	//for(int i = 0; i < 1028 / tileSize; i++) {
	//	// top walls
	//	Wall* newWall = new Wall(wallAnimSet);
	//	newWall->x = i * tileSize + tileSize; /// 2;
	//	newWall->y = tileSize / 2;
	//	walls.push_back(newWall);
	//	Entity::entities.push_back(newWall);

	//	// bottom walls
	//	newWall = new Wall(wallAnimSet); // reusing the pointer
	//	newWall->x = i * tileSize + tileSize; /// 2;
	//	newWall->y = 1028 - tileSize; /// 2;
	//	walls.push_back(newWall);
	//	Entity::entities.push_back(newWall);
	//}

	//for (int i = 0; i < 1028 / tileSize; i++) {
	//	// left walls
	//	Wall* newWall = new Wall(wallAnimSet);
	//	newWall->x = tileSize / 2;
	//	newWall->y = i * tileSize + tileSize; /// 2;
	//	walls.push_back(newWall);
	//	Entity::entities.push_back(newWall);

	//	// right walls
	//	newWall = new Wall(wallAnimSet); // reusing the pointer
	//	newWall->x = 1028 - tileSize; /// 2;
	//	newWall->y = i * tileSize + tileSize; /// 2;
	//	walls.push_back(newWall);
	//	Entity::entities.push_back(newWall);
	//}

	buildBossNext = false;
	bossActive = false;

	//build walls based on wall map
	//string line;
	//string s;
	//int yPos = 0;
	//ifstream myfile(resPath + "wallMap.txt");
	//if (myfile.is_open()){
	//	while (getline(myfile, line)){
	//		std::string::size_type pos = line.find('x');
	//		s = line.substr(0, pos);

	//		for (int i = 0; i < s.length(); i++) {
	//			if (s[i] == '1') {
	//				//teste
	//				Wall* newWall = new Wall(wallAnimSet);
	//				newWall->x = i * 32 + 16;
	//				newWall->y = yPos;
	//				walls.push_back(newWall);
	//				Entity::entities.push_back(newWall);
	//			}
	//		}

	//		yPos += 32;
	//	}

	//	myfile.close();
	//}

	//else cout << "Unable to open file";

	//teste
	Wall* newWall = new Wall(wallAnimSet);
	newWall->x = 988;
	newWall->y = 150;
	walls.push_back(newWall);
	Entity::entities.push_back(newWall);

	newWall = new Wall(wallAnimSet);
	newWall->x = 1024;
	newWall->y = 250;
	walls.push_back(newWall);
	Entity::entities.push_back(newWall);
	///////////////////////////////////

	//setup hpbars position
	heroHpBar.x = 10;
	heroHpBar.y = 10;
	heroHpBar.barHeight = 10;
	heroHpBar.barWidth = hero->hpMax * 10;
	bossHpBar.x = Globals::ScreenWidth / 2.0f - (bossHpBar.barWidth / 2.0f); // centered horizontally
	bossHpBar.y = Globals::ScreenHeight - bossHpBar.barHeight - 20; // 20 pixels off the bottom

	camController.isLerping = true;

	updateMaps();

}

Game::~Game() {
	cleanup(backGroundImage);
	cleanup(backGroundImageN);
	cleanup(backGroundImageNW);
	cleanup(backGroundImageW);
	cleanup(backGroundImageSW);
	cleanup(backGroundImageS);
	cleanup(backGroundImageSE);
	cleanup(backGroundImageE);
	cleanup(backGroundImageNE);
	cleanup(splashImage);
	cleanup(overlayImage);

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
	int enemiesToBuild = currentMap->qtEnemies;
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
						enemiesToBuild = currentMap->qtEnemies;
						enemiesBuilt = 0;
						enemyBuildTimer = 3;
						overlayTimer = 2;
						enemyWavesTillBoss = 3;
						bossActive = false; 
						buildBossNext = false;
						bossHpBar.entity = NULL; // make hpbar point to no entities

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
		}

		enemiesToBuild = currentMap->qtEnemies;

		//spawn enemies
		if (hero->hp > 0 && !splashShowing) {
			if (currentMapEnemies.size() <= 0 && mustSpawnEnemies) {
				spawnEnemies(enemiesToBuild);
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

			//if no bosses, check if we must build globs and grobs
			/*if (!buildBossNext && !bossActive && enemyBuildTimer <= 0 && enemiesBuilt < enemiesToBuild && currentMapEnemies.size() < 8) {
				float grobChance = getRandomNumber(10);

				if (grobChance < 3) {
					Grob* enemy = new Grob(grobAnimSet);
					enemy->x = getRandomNumber((Globals::ScreenWidth) - (2 * 32) - 32) + 32 + 16;
					enemy->y = getRandomNumber((Globals::ScreenHeight) - (2 * 32) - 32) + 32 + 16;
					enemy->invincibleTimer = 0.1;
					currentMapEnemies.push_back(enemy);
					Entity::entities.push_back(enemy);
					enemiesBuilt++;
					enemyBuildTimer = 1;
				}
				else {
					Glob* enemy = new Glob(globAnimSet);
					enemy->x = getRandomNumber((Globals::ScreenWidth) - (2 * 32) - 32) + 32 + 16;
					enemy->y = getRandomNumber((Globals::ScreenHeight) - (2 * 32) - 32) + 32 + 16;
					enemy->invincibleTimer = 0.1;
					currentMapEnemies.push_back(enemy);
					Entity::entities.push_back(enemy);
					enemiesBuilt++;
					enemyBuildTimer = 1;
				}
			}*/

			//boss
			if (buildBossNext && enemyBuildTimer <= 0 && currentMapEnemies.size() == 0) {
				RoundKing* round = new RoundKing(roundKingAnimSet, bulletAnimSet);
				round->invincibleTimer = 0.1;
				currentMapEnemies.push_back(round);
				Entity::entities.push_back(round);

				//make hpbar point to boss
				bossHpBar.entity = round;
				bossActive = true;
				buildBossNext = false;
				enemyWavesTillBoss = 3;
			}

			//reset spawn waves
			if (bossActive && currentMapEnemies.size() == 0) {
				bossActive = false;
				buildBossNext = false;
				enemiesBuilt = 0;
				enemiesToBuild = 2;
				bossHpBar.entity = NULL; // when boss dies, hpbar doesnt reference him anymore
			}
		}

		//If hero is in change map region, fade to change map
		if ((hero->x > currentMap->leftX1) && (hero->x < currentMap->leftX2) && (hero->y > currentMap->leftY1) && (hero->y < currentMap->leftY2)) {
			isFading = true;
			fadeIn = true;
			nextMap = NextMap::LEFT;
		}
		else if ((hero->x >= currentMap->topX1) && (hero->x < currentMap->topX2) && (hero->y >= currentMap->topY1) && (hero->y < currentMap->topY2)) {
			isFading = true;
			fadeIn = true;
			nextMap = NextMap::TOP;
		}
		else if ((hero->x >= currentMap->rightX1) && (hero->x < currentMap->rightX2) && (hero->y >= currentMap->rightY1) && (hero->y < currentMap->rightY2)) {
			isFading = true;
			fadeIn = true;
			nextMap = NextMap::RIGHT;
		}
		else if ((hero->x >= currentMap->bottomX1) && (hero->x < currentMap->bottomX2) && (hero->y >= currentMap->bottomY1) && (hero->y < currentMap->bottomY2)) {
			isFading = true;
			fadeIn = true;
			nextMap = NextMap::BOTTOM;
		}
		else {
			isFading = false;
			nextMap = NextMap::NONE;
		}

		//update camera position
		camController.update();

		// draw all entites
		draw();
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
				camController.isLerping = false;

				inactivateCurrentMapItems();

				std::ifstream ifs(getResourcePath() + "mapsPositions.json");
				json mapsPositions = json::parse(ifs);

				for (auto const& i : currentMap->itemsInMap) {
					if (i.first) {
						std::ofstream outputFile(getResourcePath() + "mapsPositions.json");
						for (auto& mapsIt : mapsPositions["maps"]["map"]) {
							int id = mapsIt["id"];
							int qt_items = mapsIt["qt_items"];
							if ((id == currentMap->id) && (qt_items > 0)) {
								for (auto& itemsIt : mapsIt["item"]) {
									int itemId = itemsIt["item_id"];
									int itemX = itemsIt["item_x"];
									int itemY = itemsIt["item_y"];

									if (itemId == get<0>(i.second) &&
										itemX == get<1>(i.second) &&
										itemY == get<2>(i.second)) {
										itemsIt["is_picked"].clear();
										itemsIt["is_picked"] = 1;
										outputFile << std::setw(4) << mapsPositions << std::endl;
									}
								}								
							}
						}
						outputFile.close();				
					}
				}

				const string& resPath = getResourcePath();

				if (nextMap == NextMap::LEFT) {
					auto tempMap = std::next(mapList.begin(), currentMap->leftMapId);
					currentMap = &(*tempMap);
					loadTiledMap(resPath + currentMap->file);
					hero->x = (hero->x - 960) + 32;
				}
				else if (nextMap == NextMap::RIGHT) {
					auto tempMap = std::next(mapList.begin(), currentMap->rightMapId);
					currentMap = &(*tempMap);
					loadTiledMap(resPath + currentMap->file);
					hero->x = (hero->x + 960) - 32;
				}
				else if (nextMap == NextMap::TOP) {
					auto tempMap = std::next(mapList.begin(), currentMap->topMapId);
					currentMap = &(*tempMap);
					loadTiledMap(resPath + currentMap->file);
					hero->y = (hero->y - 960) + 32;
				}
				else if (nextMap == NextMap::BOTTOM) {
					auto tempMap = std::next(mapList.begin(), currentMap->bottomMapId);
					currentMap = &(*tempMap);
					loadTiledMap(resPath + currentMap->file);
					hero->y = (hero->y + 960) - 32;
				}

				fadeIn = false;
				fadeOut = true;

				for (list<Entity*>::iterator enemy = currentMapEnemies.begin(); enemy != currentMapEnemies.end(); enemy++) {
					(*enemy)->active = false;
				}

				mustSpawnEnemies = true;
				hero->currentMap = currentMap;
				spawnItemsFromCurrentMap();
				camController.update();
				saveGame();
				
			}
		}
		else if (alpha >= 0 && fadeOut) {
			fadeIn = false;
			fadeOut = true;
			alphaCalc -= 2.0f;
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

	const string resPath = getResourcePath();
	string tilesetName;
	string tilesetPath;
	int x = 0;
	int y = 0;
	SDL_Texture* texture = nullptr;
	for (auto layer : currentMap->getLayers()) {
		if (&layer == nullptr) return;

		for (auto& [pos, tileObject] : layer.getTileObjects()) //Loops through absolutely all existing tiles
		{
			if (tileObject.getTile() == nullptr) return;

			if (layer.getType() == tson::LayerType::TileLayer) {
				//Set sprite data to draw the tile
				tson::Tileset* tileset = tileObject.getTile()->getTileset();
				bool hasAnimation = tileObject.getTile()->getAnimation().any();
				tson::Rect drawingRect;

				if (!hasAnimation) {
					drawingRect = tileObject.getDrawingRect();
				}

				tilesetName = tileset->getImage().filename().string();
				if (auto search = texturesCache.find(tilesetName); search != texturesCache.end()) {
					texture = search->second;
				}
				else {
					tilesetPath = resPath + "Assets\\Textures\\" + tileset->getImage().filename().string();
					texture = loadTexture(tilesetPath, Globals::renderer);
					texturesCache.emplace(tilesetName, texture);
				}

				if (texture == nullptr) return;

				SDL_Rect tileRect;
				tileRect.x = tileObject.getDrawingRect().x;
				tileRect.y = tileObject.getDrawingRect().y;
				tileRect.w = tileObject.getDrawingRect().width;
				tileRect.h = tileObject.getDrawingRect().height;

				SDL_Rect renderTile;
				renderTile.x = (x * 32) - Globals::camera.x;
				renderTile.y = (y * 32) - Globals::camera.y;
				renderTile.w = 32;
				renderTile.h = 32;

				SDL_RenderCopy(Globals::renderer, texture, &tileRect, &renderTile);

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

void Game::draw() {
	// clear screen
	SDL_SetRenderDrawColor(Globals::renderer, 145, 133, 129, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(Globals::renderer);

	//TESTE
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

		//TODO: Encapsular desenho de UI
		quickItemUi->draw();
		itemPickMessageUi->draw();

		//draw UI stuff
		heroHpBar.draw();
		bossHpBar.draw();

		if (overlayTimer <= 0 && hero->hp < 1) {
			renderTexture(overlayImage, Globals::renderer, 0, 0);

			if (scoreTexture == NULL) {
				//generate score text
				SDL_Color color = { 255, 255, 255, 255 };

				stringstream ss;
				ss << "Enemies dispatched: " << Glob::globsKilled + Grob::grobsKilled + RoundKing::roundKingsKilled;

				string resPath = getResourcePath();
				scoreTexture = renderText(ss.str(), resPath + "vermin_vibes_1989.ttf", color, 30, Globals::renderer);
			}

			renderTexture(scoreTexture, Globals::renderer, 20, 50);
		}
	}

	// after done drawing, show it to the screen
	SDL_RenderPresent(Globals::renderer);
}

void Game::spawnEnemies(int enemiesToBuild) {
	int i = 0;
	int enemyId;
	int enemyPosX, enemyPosY;
	int uniqueId;
	while (i < enemiesToBuild) {
		enemyId = std::get<0>(currentMap->enemies[i]);
		enemyPosX = std::get<1>(currentMap->enemies[i]);
		enemyPosY = std::get<2>(currentMap->enemies[i]);
		uniqueId = (currentMap->id * 100) + (enemyId * 10) + i;
		switch (enemyId) {
		case 0:
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

		case 1:
			if (deadEnemiesIds.empty() ||
				std::find(deadEnemiesIds.begin(), deadEnemiesIds.end(), uniqueId) == deadEnemiesIds.end()) {
				TermiteMiner* enemy = new TermiteMiner(termiteMinerAnimSet);
				enemy->x = enemyPosX;
				enemy->y = enemyPosY;
				enemy->invincibleTimer = 0.1;
				enemy->enemyId = uniqueId;
				currentMapEnemies.push_back(enemy);
				Entity::entities.push_back(enemy);

				//Grob* enemy = new Grob(grobAnimSet);
				//enemy->x = enemyPosX;
				//enemy->y = enemyPosY;
				//enemy->invincibleTimer = 0.1;
				//enemy->enemyId = uniqueId;
				//currentMapEnemies.push_back(enemy);
				//Entity::entities.push_back(enemy);
			}
			break;
		}
		
		i++;
	}
	mustSpawnEnemies = false;
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
	hDewPotionAnimSet->loadAnimationSet("groundConsumableItem.fdset", dataGroupTypes);

	heroAnimSet = new AnimationSet();
	heroAnimSet->loadAnimationSet("antHero.fdset", dataGroupTypes, true, 0, true);//"udemyCyborg.fdset", dataGroupTypes, true, 0, true);

	globAnimSet = new AnimationSet();
	globAnimSet->loadAnimationSet("glob.fdset", dataGroupTypes, true, 0, true);

	grobAnimSet = new AnimationSet();
	grobAnimSet->loadAnimationSet("grob.fdset", dataGroupTypes, true, 0, true);

	termiteMinerAnimSet = new AnimationSet();
	termiteMinerAnimSet->loadAnimationSet("termiteMiner.fdset", dataGroupTypes, true, 0, true);

	wallAnimSet = new AnimationSet();
	wallAnimSet->loadAnimationSet("wall.fdset", dataGroupTypes);

	roundKingAnimSet = new AnimationSet();
	roundKingAnimSet->loadAnimationSet("roundKing.fdset", dataGroupTypes, true, 0, true);

	bulletAnimSet = new AnimationSet();
	bulletAnimSet->loadAnimationSet("bullet.fdset", dataGroupTypes, true, 0, true);

	stoneProjectileAnimSet = new AnimationSet();
	stoneProjectileAnimSet->loadAnimationSet("stoneProjectile.fdset", dataGroupTypes, true, 0, true);
}

void Game::spawnItemsFromCurrentMap(){
	for (auto const& i : currentMap->itemsInMap) {
		//TODO: Acrescentar quantidade de itens no maps.xml e no itemsInMap
		if (!i.first) {
			spawnItem(get<0>(i.second), 1, get<1>(i.second), get<2>(i.second));
		}
	}
}

void Game::inactivateCurrentMapItems(){
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
	saveHandler.save(hero->hp, hero->x, hero->y, currentMap->id, inventory);
}

void Game::loadGame() {
	//TODO: Trazer rotina de carregamento
}
