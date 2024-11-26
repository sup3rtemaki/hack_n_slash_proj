#include "ui/menu/pauseMenu.h"

#include "hero.h"

PauseMenu::PauseMenu(Hero* hero) {
	this->hero = hero;
	setUp();
}

void PauseMenu::draw() {
}

void PauseMenu::setUp() {
	__super::setUp();
}

void PauseMenu::drawMenuBackground() {
}

void PauseMenu::drawText() {
}

void PauseMenu::drawSelectionBox() {
}
