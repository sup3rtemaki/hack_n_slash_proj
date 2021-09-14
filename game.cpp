#include "game.h"

Game::Game() {
	string resPath = getResourcePath();
	backGroundImage = loadTexture(resPath + "map.png", Globals::renderer);
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

	song = Mix_LoadMUS(string(resPath + "Fatal Theory.wav").c_str());
	if (song != NULL) {
		Mix_PlayMusic(song, -1);
		Mix_Volume(-1, 50);
	}

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
	hero->x = Globals::ScreenWidth / 2;
	hero->y = Globals::ScreenHeight / 2;
	heroInput.hero = hero;
	heroHpBar.entity = hero;
	Entity::entities.push_back(hero);

	//get camera to follow hero
	camController.target = hero;

	// build the walls
	int tileSize = 32;

	for (int i = 0; i < Globals::ScreenWidth / tileSize; i++) {
		// top walls
		Wall* newWall = new Wall(wallAnimSet);
		newWall->x = i * tileSize + tileSize / 2;
		newWall->y = tileSize / 2;
		walls.push_back(newWall);
		Entity::entities.push_back(newWall);

		// bottom walls
		newWall = new Wall(wallAnimSet); // reusing the pointer
		newWall->x = i * tileSize + tileSize / 2;
		newWall->y = Globals::ScreenHeight - tileSize / 2;
		walls.push_back(newWall);
		Entity::entities.push_back(newWall);
	}

	for (int i = 1; i < Globals::ScreenHeight / tileSize - 1; i++) {
		// left walls
		Wall* newWall = new Wall(wallAnimSet);
		newWall->x = tileSize / 2;
		newWall->y = i * tileSize + tileSize / 2;
		walls.push_back(newWall);
		Entity::entities.push_back(newWall);

		// right walls
		newWall = new Wall(wallAnimSet); // reusing the pointer
		newWall->x = Globals::ScreenWidth - tileSize / 2;
		newWall->y = i * tileSize + tileSize / 2;
		walls.push_back(newWall);
		Entity::entities.push_back(newWall);
	}

	buildBossNext = false;
	bossActive = false;

	//setup hpbars position
	heroHpBar.x = 10;
	heroHpBar.y = 10;
	heroHpBar.barHeight = 10;
	heroHpBar.barWidth = 100;
	bossHpBar.x = Globals::ScreenWidth / 2.0f - (bossHpBar.barWidth / 2.0f); // centered horizontally
	bossHpBar.y = Globals::ScreenHeight - bossHpBar.barHeight - 20; // 20 pixels off the bottom

	////teste
	//Wall* newWall = new Wall(wallAnimSet);
	//newWall->x = Globals::ScreenWidth / 4;
	//newWall->y = Globals::ScreenHeight / 2;
	//walls.push_back(newWall);
	//Entity::entities.push_back(newWall);
}

Game::~Game() {
	cleanup(backGroundImage);
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
						enemiesToBuild = 2;
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
				enemiesToBuild = enemiesToBuild + 2;
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
					enemy->x = getRandomNumber(Globals::ScreenWidth - (2 * 32) - 32) + 32 + 16;
					enemy->y = getRandomNumber(Globals::ScreenHeight - (2 * 32) - 32) + 32 + 16;
					enemy->invincibleTimer = 0.1;
					enemies.push_back(enemy);
					Entity::entities.push_back(enemy);
					enemiesBuilt++;
					enemyBuildTimer = 1;
				}
				else {
					Glob* enemy = new Glob(globAnimSet);
					enemy->x = getRandomNumber(Globals::ScreenWidth - (2 * 32) - 32) + 32 + 16;
					enemy->y = getRandomNumber(Globals::ScreenHeight - (2 * 32) - 32) + 32 + 16;
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

		//update camera position
		camController.update();

		// draw all entites
		draw();
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

		// draw backgorund
		renderTexture(backGroundImage, Globals::renderer, 0 - Globals::camera.x, 0 - Globals::camera.y);

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