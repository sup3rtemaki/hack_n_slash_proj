#include "game.h"
#include "tinyxml2.h"
#include <iostream>
#include <fstream>
#include <string>


#define MAP1_X 1025
#define MAP1_Y 0
#define MAP_DISTANCE 2000

using namespace std;

Game::Game() {

	string resPath = getResourcePath();

	//teste
	backGroundImage = loadTexture(resPath + "map.png", Globals::renderer);
	fadeImage = loadTexture(resPath + "blackBG.png", Globals::renderer);

	currentMapId = 1;
	mapToDrawCount = 0;

	tinyxml2::XMLDocument xml_doc;

	tinyxml2::XMLError mResult = xml_doc.LoadFile("mapsPositions.xml");

	if (mResult == tinyxml2::XML_SUCCESS) {
		tinyxml2::XMLNode* root = xml_doc.FirstChildElement("maps");

		tinyxml2::XMLElement* element = root->FirstChildElement("map_count");
		int xml_count;
		mResult = element->QueryIntText(&xml_count);
		cout << element->GetText() << "\n";

		//list<Map> mapList;

		element = root->FirstChildElement("map");
		int i = 0;
		
		while (element != nullptr && i < xml_count)
		{
			tinyxml2::XMLElement* item = element->FirstChildElement("file");
			string f = item->GetText();

			int mId;
			item = element->FirstChildElement("id");
			mResult = item->QueryIntText(&mId);

			int x;
			item = element->FirstChildElement("pos_x");
			mResult = item->QueryIntText(&x);

			int y;
			item = element->FirstChildElement("pos_y");
			mResult = item->QueryIntText(&y);

			int mN;
			item = element->FirstChildElement("map_n");
			mResult = item->QueryIntText(&mN);

			int mNW;
			item = element->FirstChildElement("map_nw");
			mResult = item->QueryIntText(&mNW);

			int mW;
			item = element->FirstChildElement("map_w");
			mResult = item->QueryIntText(&mW);

			int mSW;
			item = element->FirstChildElement("map_sw");
			mResult = item->QueryIntText(&mSW);

			int mS;
			item = element->FirstChildElement("map_s");
			mResult = item->QueryIntText(&mS);

			int mSE;
			item = element->FirstChildElement("map_se");
			mResult = item->QueryIntText(&mSE);

			int mE;
			item = element->FirstChildElement("map_e");
			mResult = item->QueryIntText(&mE);

			int mNE;
			item = element->FirstChildElement("map_ne");
			mResult = item->QueryIntText(&mNE);

			
			Map m = Map(mId, f, x, y, mN, mNW, mW, mSW, mS, mSE, mE, mNE);
			mapList.push_back(m);

			cout << m.id << " " << m.file << " " << m.mapPosX << " " << m.mapPosY << " " <<
				m.mapN << " " << m.mapNW << " " << m.mapW << " " << m.mapSW << " " 
				<< m.mapS << " " << m.mapSE << " " << m.mapE << " " << m.mapNE << "\n";

			element = element->NextSiblingElement("map");
			i++;
		}
	}
	else {
		cout << "Error opening XML";
	}

	auto tempMap = std::next(mapList.begin(), currentMapId); //Get the current map based on currentMapId

	currentMap = Map(tempMap->id, tempMap->file, tempMap->mapPosX, tempMap->mapPosY,
		tempMap->mapN, tempMap->mapNW, tempMap->mapW, tempMap->mapSW,
		tempMap->mapS, tempMap->mapSE, tempMap->mapE, tempMap->mapNE);

	//Pre-load current and surroundings maps images
	//backGroundImage = loadTexture(resPath + currentMap.file, Globals::renderer);
	cout << currentMap.file << "\n";

	tempMap = std::next(mapList.begin(), currentMap.mapN);
	backGroundImageN = loadTexture(resPath + tempMap->file, Globals::renderer);

	tempMap = std::next(mapList.begin(), currentMap.mapNW);
	backGroundImageNW = loadTexture(resPath + tempMap->file, Globals::renderer);

	tempMap = std::next(mapList.begin(), currentMap.mapW);
	backGroundImageW = loadTexture(resPath + tempMap->file, Globals::renderer);

	tempMap = std::next(mapList.begin(), currentMap.mapSW);
	backGroundImageSW = loadTexture(resPath + tempMap->file, Globals::renderer);

	tempMap = std::next(mapList.begin(), currentMap.mapS);
	backGroundImageS = loadTexture(resPath + tempMap->file, Globals::renderer);

	tempMap = std::next(mapList.begin(), currentMap.mapSE);
	backGroundImageSE = loadTexture(resPath + tempMap->file, Globals::renderer);

	tempMap = std::next(mapList.begin(), currentMap.mapE);
	backGroundImageE = loadTexture(resPath + tempMap->file, Globals::renderer);

	tempMap = std::next(mapList.begin(), currentMap.mapNE);
	backGroundImageNE = loadTexture(resPath + tempMap->file, Globals::renderer);

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

	heroAnimSet = new AnimationSet();
	heroAnimSet->loadAnimationSet("udemyCyborg.fdset", dataGroupTypes, true, 0, true);

	globAnimSet = new AnimationSet();
	globAnimSet->loadAnimationSet("glob.fdset", dataGroupTypes, true, 0, true);

	grobAnimSet = new AnimationSet();
	grobAnimSet->loadAnimationSet("grob.fdset", dataGroupTypes, true, 0, true);

	wallAnimSet = new AnimationSet();
	wallAnimSet->loadAnimationSet("wall.fdset", dataGroupTypes);

	roundKingAnimSet = new AnimationSet();
	roundKingAnimSet->loadAnimationSet("roundKing.fdset", dataGroupTypes, true, 0, true);

	bulletAnimSet = new AnimationSet();
	bulletAnimSet->loadAnimationSet("bullet.fdset", dataGroupTypes, true, 0, true);

	// build hero entity
	hero = new Hero(heroAnimSet);
	hero->invincibleTimer = 0;
	hero->x = 0;//Globals::ScreenWidth / 2;
	hero->y = 0;//Globals::ScreenHeight / 2;
	heroInput.hero = hero;
	heroHpBar.entity = hero;
	Entity::entities.push_back(hero);

	//get camera to follow hero
	camController.target = hero;

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

	//setup hpbars position
	heroHpBar.x = 10;
	heroHpBar.y = 10;
	heroHpBar.barHeight = 10;
	heroHpBar.barWidth = 100;
	bossHpBar.x = Globals::ScreenWidth / 2.0f - (bossHpBar.barWidth / 2.0f); // centered horizontally
	bossHpBar.y = Globals::ScreenHeight - bossHpBar.barHeight - 20; // 20 pixels off the bottom
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

	Mix_PausedMusic();
	Mix_FreeMusic(song);

	Entity::removeAllFromList(&Entity::entities, false);
	delete heroAnimSet;
	delete globAnimSet;
	delete grobAnimSet;
	delete wallAnimSet;
	delete roundKingAnimSet;
	delete bulletAnimSet;
	delete hero;
	Entity::removeAllFromList(&walls, true);
	Entity::removeAllFromList(&enemies, true);
}

void Game::update() {
	int enemiesToBuild = 2;
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

		//remove and delete enemies in the list who are dead/inactive
		Entity::removeInactiveEntitiesFromList(&enemies, true);
		
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
						enemiesToBuild = 6;
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

						//remove existing enemies
						for (list<Entity*>::iterator enemy = enemies.begin(); enemy != enemies.end(); enemy++) {
							(*enemy)->active = false;
						}
						hero->revive();
					}
					
					break;
				}
			}
			heroInput.update(&e);
		}

		//make overlay timer tick down
		if (hero->hp < 1 && overlayTimer > 0) {
			overlayTimer -= TimeController::timeController.dT;
		}

		// update all entites
		for (list<Entity*>::iterator entity = Entity::entities.begin(); entity != Entity::entities.end(); entity++) {
			// update all entites in world at once (polymorphism)
			(*entity)->update();
		}

		//spawn enemies
		if (hero->hp > 0 && !splashShowing) {
			if (enemiesToBuild == enemiesBuilt && enemies.size() <= 0) {
				enemiesToBuild = enemiesToBuild + 6;
				enemiesBuilt = 0;
				enemyBuildTimer = 4;
				enemyWavesTillBoss--;

				if (enemyWavesTillBoss <= 0) {
					buildBossNext = true;
				}
			}
			enemyBuildTimer -= TimeController::timeController.dT;
			//if no bosses, check if we must build globs and grobs
			if (!buildBossNext && !bossActive && enemyBuildTimer <= 0 && enemiesBuilt < enemiesToBuild && enemies.size() < 8) {
				float grobChance = getRandomNumber(10);

				if (grobChance < 3) {
					Grob* enemy = new Grob(grobAnimSet);
					enemy->x = getRandomNumber((Globals::ScreenWidth) - (2 * 32) - 32) + 32 + 16;
					enemy->y = getRandomNumber((Globals::ScreenHeight) - (2 * 32) - 32) + 32 + 16;
					enemy->invincibleTimer = 0.1;
					enemies.push_back(enemy);
					Entity::entities.push_back(enemy);
					enemiesBuilt++;
					enemyBuildTimer = 1;
				}
				else {
					Glob* enemy = new Glob(globAnimSet);
					enemy->x = getRandomNumber((Globals::ScreenWidth) - (2 * 32) - 32) + 32 + 16;
					enemy->y = getRandomNumber((Globals::ScreenHeight) - (2 * 32) - 32) + 32 + 16;
					enemy->invincibleTimer = 0.1;
					enemies.push_back(enemy);
					Entity::entities.push_back(enemy);
					enemiesBuilt++;
					enemyBuildTimer = 1;
				}
			}

			//boss
			if (buildBossNext && enemyBuildTimer <= 0 && enemies.size() == 0) {
				RoundKing* round = new RoundKing(roundKingAnimSet, bulletAnimSet);
				round->invincibleTimer = 0.1;
				enemies.push_back(round);
				Entity::entities.push_back(round);

				//make hpbar point to boss
				bossHpBar.entity = round;

				bossActive = true;
				buildBossNext = false;
				enemyWavesTillBoss = 3;
			}

			//reset spawn waves
			if (bossActive && enemies.size() == 0) {
				bossActive = false;
				buildBossNext = false;
				enemiesBuilt = 0;
				enemiesToBuild = 2;
				bossHpBar.entity = NULL; // when boss dies, hpbar doesnt reference him anymore
			}
		}

		//TESTE
		if ((hero->x > 500) && (hero->y > 0 || hero->y < 15)) {
			isFading = true;
			fadeIn = true;
		}

		//update camera position
		camController.update();

		// draw all entites
		draw();
	}
}

void Game::updateMaps() {
	auto tempMap = std::next(mapList.begin(), currentMapId);

	if ((hero->x > (currentMap.mapPosX + 1024)) && (hero->y > (currentMap.mapPosY + 1024)) && (currentMap.mapSE != 0)) {
		//Hero is SE
		currentMap = *tempMap;
		mapToDrawCount = 0;

		currentMapId = currentMap.mapSE;
		tempMap = std::next(mapList.begin(), currentMapId);
		currentMap = *tempMap;

		backGroundImage = loadTexture(getResourcePath() + currentMap.file, Globals::renderer);
	}
	else if (hero->x > (currentMap.mapPosX + 1024) && (currentMap.mapE != 0)) {
		//Hero is E
		currentMap = *tempMap;
		mapToDrawCount = 0;

		currentMapId = currentMap.mapE;
		tempMap = std::next(mapList.begin(), currentMapId);
		currentMap = *tempMap;

		backGroundImage = loadTexture(getResourcePath() + currentMap.file, Globals::renderer);
	}
	else if (hero->y > (currentMap.mapPosY + 1024) && (currentMap.mapS != 0)) {
		//Hero is S
		currentMap = *tempMap;
		mapToDrawCount = 0;

		currentMapId = currentMap.mapS;
		tempMap = std::next(mapList.begin(), currentMapId);
		currentMap = *tempMap;

		backGroundImage = loadTexture(getResourcePath() + currentMap.file, Globals::renderer);
	}
	else if ((hero->x < currentMap.mapPosX) && (hero->y > (currentMap.mapPosY + 1024)) && (currentMap.mapSW != 0)) {
		//Hero is SW
		currentMap = *tempMap;
		mapToDrawCount = 0;

		currentMapId = currentMap.mapSW;
		tempMap = std::next(mapList.begin(), currentMapId);
		currentMap = *tempMap;

		backGroundImage = loadTexture(getResourcePath() + currentMap.file, Globals::renderer);
	}
	else if ((hero->x < currentMap.mapPosX) && (currentMap.mapW != 0)) {
		//Hero is W
		currentMap = *tempMap;
		mapToDrawCount = 0;

		currentMapId = currentMap.mapW;
		tempMap = std::next(mapList.begin(), currentMapId);
		currentMap = *tempMap;

		backGroundImage = loadTexture(getResourcePath() + currentMap.file, Globals::renderer);
	}
	else if ((hero->x < currentMap.mapPosX) && (hero->y < currentMap.mapPosY) && (currentMap.mapNW != 0)) {
		//Hero is NW
		currentMap = *tempMap;
		mapToDrawCount = 0;

		currentMapId = currentMap.mapNW;
		tempMap = std::next(mapList.begin(), currentMapId);
		currentMap = *tempMap;

		backGroundImage = loadTexture(getResourcePath() + currentMap.file, Globals::renderer);
	}
	else if ((hero->y < currentMap.mapPosY) && (currentMap.mapN != 0)) {
		//Hero is N
		currentMap = *tempMap;
		mapToDrawCount = 0;

		currentMapId = currentMap.mapN;
		tempMap = std::next(mapList.begin(), currentMapId);
		currentMap = *tempMap;

		backGroundImage = loadTexture(getResourcePath() + currentMap.file, Globals::renderer);
	}
	else if ((hero->x > (currentMap.mapPosX + 1024)) && (hero->y < currentMap.mapPosY) && (currentMap.mapNE != 0)) {
		//Hero is NE
		currentMap = *tempMap;
		mapToDrawCount = 0;

		currentMapId = currentMap.mapNE;
		tempMap = std::next(mapList.begin(), currentMapId);
		currentMap = *tempMap;

		backGroundImage = loadTexture(getResourcePath() + currentMap.file, Globals::renderer);
	}

	//Draw current map
	renderTexture(backGroundImage, Globals::renderer, currentMap.mapPosX - Globals::camera.x, currentMap.mapPosY - Globals::camera.y);

	//Load and draw surrounding maps based on map change
	if ((lastMapId != currentMapId) && mapToDrawCount < 8) {

		//FSM
		switch (mapToDrawCount)
		{
		case 0:
			tempMap = std::next(mapList.begin(), currentMap.mapN);
			backGroundImageN = loadTexture(getResourcePath() + tempMap->file, Globals::renderer);
			//renderTexture(backGroundImageN, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);
			mapToDrawCount++;
			break;
		case 1:
			tempMap = std::next(mapList.begin(), currentMap.mapNW);
			backGroundImageNW = loadTexture(getResourcePath() + tempMap->file, Globals::renderer);
			//renderTexture(backGroundImageNW, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);
			mapToDrawCount++;
			break;
		case 2:
			tempMap = std::next(mapList.begin(), currentMap.mapW);
			backGroundImageW = loadTexture(getResourcePath() + tempMap->file, Globals::renderer);
			//renderTexture(backGroundImageW, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);
			mapToDrawCount++;
			break;
		case 3:
			tempMap = std::next(mapList.begin(), currentMap.mapSW);
			backGroundImageSW = loadTexture(getResourcePath() + tempMap->file, Globals::renderer);
			//renderTexture(backGroundImageSW, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);
			mapToDrawCount++;
			break;
		case 4:
			tempMap = std::next(mapList.begin(), currentMap.mapS);
			backGroundImageS = loadTexture(getResourcePath() + tempMap->file, Globals::renderer);
			//renderTexture(backGroundImageS, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);
			mapToDrawCount++;
			break;
		case 5:
			tempMap = std::next(mapList.begin(), currentMap.mapSE);
			backGroundImageSE = loadTexture(getResourcePath() + tempMap->file, Globals::renderer);
			//renderTexture(backGroundImageSE, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);
			mapToDrawCount++;
			break;
		case 6:
			tempMap = std::next(mapList.begin(), currentMap.mapE);
			backGroundImageE = loadTexture(getResourcePath() + tempMap->file, Globals::renderer);
			//renderTexture(backGroundImageE, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);
			mapToDrawCount++;
			break;
		case 7:
			tempMap = std::next(mapList.begin(), currentMap.mapNE);
			backGroundImageNE = loadTexture(getResourcePath() + tempMap->file, Globals::renderer);
			//renderTexture(backGroundImageNE, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);
			mapToDrawCount++;
			lastMapId = currentMapId;
			break;
		default:
			break;
		}
	}
	else {
		tempMap = std::next(mapList.begin(), currentMap.mapN);
		renderTexture(backGroundImageN, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);

		tempMap = std::next(mapList.begin(), currentMap.mapNW);
		renderTexture(backGroundImageNW, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);

		tempMap = std::next(mapList.begin(), currentMap.mapW);
		renderTexture(backGroundImageW, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);

		tempMap = std::next(mapList.begin(), currentMap.mapSW);
		renderTexture(backGroundImageSW, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);

		tempMap = std::next(mapList.begin(), currentMap.mapS);
		renderTexture(backGroundImageS, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);

		tempMap = std::next(mapList.begin(), currentMap.mapSE);
		renderTexture(backGroundImageSE, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);

		tempMap = std::next(mapList.begin(), currentMap.mapE);
		renderTexture(backGroundImageE, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);

		tempMap = std::next(mapList.begin(), currentMap.mapNE);
		renderTexture(backGroundImageNE, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);
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

		if (isFading) {

			if (alpha < 255 && fadeIn) {
				fadeIn = true;
				fadeOut = false;
				alphaCalc += 3.0f;
				alpha = alphaCalc;
				SDL_SetTextureAlphaMod(fadeImage, alpha);

				if (alpha > 254) {
					hero->x = 300;
					hero->y = 300;
					fadeIn = false;
					fadeOut = true;
				}
			}
			else if (alpha >= 0 && fadeOut) {
				fadeIn = false;
				fadeOut = true;
				alphaCalc -= 3.0f;
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
			cout << alpha << "\n";
		}
		else {
			SDL_SetTextureAlphaMod(fadeImage, 0);
			alpha = 0;
			alphaCalc = 0.0f;
		}

		int i = 1;

		renderTexture(backGroundImage, Globals::renderer, 0 - Globals::camera.x, 0 - Globals::camera.y);
		renderTexture(fadeImage, Globals::renderer, (-200) - Globals::camera.x, (-200) - Globals::camera.y);
		//updateMaps();

		//Load and draw surrounding maps based on distance
		//tempMap = std::next(mapList.begin(), currentMap.mapN);
		//if ((Entity::distaceBettweenTwoPoints(hero->x, hero->y, tempMap->mapPosX, tempMap->mapPosY) < MAP_DISTANCE) 
		//	&& ((tempMap->mapPosX != 1) && (tempMap->mapPosY != 1))) {
		//	backGroundImageN = loadTexture(getResourcePath() + tempMap->file, Globals::renderer);
		//	renderTexture(backGroundImageN, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);
		//}

		//tempMap = std::next(mapList.begin(), currentMap.mapNW);
		//if ((Entity::distaceBettweenTwoPoints(hero->x, hero->y, tempMap->mapPosX, tempMap->mapPosY) < MAP_DISTANCE)
		//	&& ((tempMap->mapPosX != 1) && (tempMap->mapPosY != 1))) {
		//	backGroundImageNW = loadTexture(getResourcePath() + tempMap->file, Globals::renderer);
		//	renderTexture(backGroundImageNW, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);
		//}

		//tempMap = std::next(mapList.begin(), currentMap.mapW);
		//if ((Entity::distaceBettweenTwoPoints(hero->x, hero->y, tempMap->mapPosX, tempMap->mapPosY) < MAP_DISTANCE)
		//	&& ((tempMap->mapPosX != 1) && (tempMap->mapPosY != 1))) {
		//	backGroundImageW = loadTexture(getResourcePath() + tempMap->file, Globals::renderer);
		//	renderTexture(backGroundImageW, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);
		//}

		//tempMap = std::next(mapList.begin(), currentMap.mapSW);
		//if ((Entity::distaceBettweenTwoPoints(hero->x, hero->y, tempMap->mapPosX, tempMap->mapPosY) < MAP_DISTANCE)
		//	&& ((tempMap->mapPosX != 1) && (tempMap->mapPosY != 1))) {
		//	backGroundImageSW = loadTexture(getResourcePath() + tempMap->file, Globals::renderer);
		//	renderTexture(backGroundImageSW, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);
		//}

		//tempMap = std::next(mapList.begin(), currentMap.mapS);
		//if ((Entity::distaceBettweenTwoPoints(hero->x, hero->y, tempMap->mapPosX, tempMap->mapPosY) < MAP_DISTANCE)
		//	&& ((tempMap->mapPosX != 1) && (tempMap->mapPosY != 1))) {
		//	backGroundImageS = loadTexture(getResourcePath() + tempMap->file, Globals::renderer);
		//	renderTexture(backGroundImageS, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);
		//}

		//tempMap = std::next(mapList.begin(), currentMap.mapSE);
		//if ((Entity::distaceBettweenTwoPoints(hero->x, hero->y, tempMap->mapPosX, tempMap->mapPosY) < MAP_DISTANCE)
		//	&& ((tempMap->mapPosX != 1) && (tempMap->mapPosY != 1))) {
		//	backGroundImageSE = loadTexture(getResourcePath() + tempMap->file, Globals::renderer);
		//	renderTexture(backGroundImageSE, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);
		//}

		//tempMap = std::next(mapList.begin(), currentMap.mapE);
		//if ((Entity::distaceBettweenTwoPoints(hero->x, hero->y, tempMap->mapPosX, tempMap->mapPosY) < MAP_DISTANCE)
		//	&& ((tempMap->mapPosX != 1) && (tempMap->mapPosY != 1))) {
		//	backGroundImageE = loadTexture(getResourcePath() + tempMap->file, Globals::renderer);
		//	renderTexture(backGroundImageE, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);
		//}

		//tempMap = std::next(mapList.begin(), currentMap.mapNE);
		//if ((Entity::distaceBettweenTwoPoints(hero->x, hero->y, tempMap->mapPosX, tempMap->mapPosY) < MAP_DISTANCE)
		//	&& ((tempMap->mapPosX != 1) && (tempMap->mapPosY != 1))) {
		//	backGroundImageNE = loadTexture(getResourcePath() + tempMap->file, Globals::renderer);
		//	renderTexture(backGroundImageNE, Globals::renderer, tempMap->mapPosX - Globals::camera.x, tempMap->mapPosY - Globals::camera.y);
		//}


		//if(Entity::distaceBettweenTwoPoints)
		//if (sqrt(pow(MAP1_X - hero->x, 2) + pow(MAP1_Y - hero->y, 2)) < MAP_DISTANCE) {
		//	backGroundImage2 = loadTexture(getResourcePath() + "map.png", Globals::renderer);
		//	renderTexture(backGroundImage2, Globals::renderer, MAP1_X - Globals::camera.x, MAP1_Y - Globals::camera.y);
		//}
		//else {
		//	cleanup(backGroundImage2);
		//}
		
		//renderTexture(backGroundImage, Globals::renderer, 1024 - Globals::camera.x, -1024 - Globals::camera.y);
		//renderTexture(backGroundImage, Globals::renderer, -1024 - Globals::camera.x, 1024 - Globals::camera.y);
		//renderTexture(backGroundImage, Globals::renderer, -1024 - Globals::camera.x, 0 - Globals::camera.y);
		//renderTexture(backGroundImage, Globals::renderer, 0 - Globals::camera.x, -1024 - Globals::camera.y);
		//renderTexture(backGroundImage, Globals::renderer, -1024 - Globals::camera.x, -1024 - Globals::camera.y);
		//renderTexture(backGroundImage, Globals::renderer, 1024 - Globals::camera.x, 0 - Globals::camera.y);
		//renderTexture(backGroundImage, Globals::renderer, 0 - Globals::camera.x, 1024 - Globals::camera.y);
		//renderTexture(backGroundImage, Globals::renderer, 1024 - Globals::camera.x, 1024 - Globals::camera.y);

		// sort all entities based on y (depth)
		Entity::entities.sort(Entity::EntityCompare);

		// draw all of the entities
		for (list<Entity*>::iterator entity = Entity::entities.begin(); entity != Entity::entities.end(); entity++) {
			(*entity)->draw();
		}

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