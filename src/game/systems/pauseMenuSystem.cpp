#include "pauseMenuSystem.h"
#include "pauseMenu.h"
#include "timeController.h"
#include "renderContext.h"

PauseMenuSystem::PauseMenuSystem(PauseMenu* pauseMenu, TimeController& gameTime, RenderContext& renderContext)
	: pauseMenu(pauseMenu), gameTime(gameTime), renderContext(renderContext) {
}

PauseMenuSystem::~PauseMenuSystem() {
}

bool PauseMenuSystem::update(SDL_Event& event) {
	// Process single event
	if (event.type == SDL_QUIT) {
		if (checkQuitCallback) {
			checkQuitCallback();
		}
		return false; // don't resume
	}

	// keydown event
	if (event.type == SDL_KEYDOWN) {
		handleKeyDown(event.key.keysym.scancode);
	}

	return false; // stays paused
}

void PauseMenuSystem::handleKeyDown(SDL_Keycode keycode) {
	switch (keycode) {
	case SDL_SCANCODE_ESCAPE:
		// Verifica se está no modo de seleção de quick slot
		if (pauseMenu->inventoryMode == InventoryMode::SelectingQuickSlot) {
			pauseMenu->cancelQuickSlotSelection();
			break;
		}

		switch (pauseMenu->menuState) {
		case MenuState::Active:
			pauseMenu->menuState = MenuState::Inactive;
			if (resumeGameCallback) {
				resumeGameCallback();
			}
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

void PauseMenuSystem::render() {
	SDL_SetRenderDrawColor(renderContext.renderer, 145, 133, 129, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderContext.renderer);
	pauseMenu->draw();
	SDL_RenderPresent(renderContext.renderer);
}

void PauseMenuSystem::setResumeGameCallback(std::function<void()> callback) {
	resumeGameCallback = callback;
}

void PauseMenuSystem::setQuitGameCallback(std::function<void()> callback) {
	quitGameCallback = callback;
}

void PauseMenuSystem::setCheckQuitCallback(std::function<bool()> callback) {
	checkQuitCallback = callback;
}

void PauseMenuSystem::setQuitFlagCallback(std::function<void(bool)> callback) {
	quitFlagCallback = callback;
}
