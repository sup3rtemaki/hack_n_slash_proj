#include "mainMenuSystem.h"
#include "mainMenu.h"
#include "renderContext.h"
#include "globals.h"

MainMenuSystem::MainMenuSystem(MainMenu* mainMenu, RenderContext& renderContext)
	: mainMenu(mainMenu), renderContext(renderContext) {
}

MainMenuSystem::~MainMenuSystem() {
}

bool MainMenuSystem::update(SDL_Event& event) {
	// Process single event
	if (event.type == SDL_QUIT) {
		if (quitCallback) {
			quitCallback();
		}
		return false;
	}

	// keydown event
	if (event.type == SDL_KEYDOWN) {
		handleKeyDown(event.key.keysym.scancode);
	}

	render();
	return false;
}

void MainMenuSystem::handleKeyDown(SDL_Keycode keycode) {
	switch (keycode) {
	case SDL_SCANCODE_UP:
		mainMenu->onIndexUp();
		break;
	case SDL_SCANCODE_DOWN:
		mainMenu->onIndexDown();
		break;
	case SDL_SCANCODE_ESCAPE:
		if (quitCallback) {
			quitCallback();
		}
		break;
	case SDL_SCANCODE_SPACE:
		switch (mainMenu->index) {
		case 0: // Start Game
			if (startGameCallback) {
				startGameCallback();
			}
			break;
		case 1: // Quit
			if (quitCallback) {
				quitCallback();
			}
			break;
		}
		break;
	}
}

void MainMenuSystem::render() {
	SDL_SetRenderDrawColor(renderContext.renderer, 145, 133, 129, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderContext.renderer);
	mainMenu->draw();
	SDL_RenderPresent(renderContext.renderer);
}

void MainMenuSystem::setStartGameCallback(std::function<void()> callback) {
	startGameCallback = callback;
}

void MainMenuSystem::setQuitCallback(std::function<void()> callback) {
	quitCallback = callback;
}
