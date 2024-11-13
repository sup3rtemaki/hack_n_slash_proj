#include "ui/menu/menu.h"

#include "hero.h"
#include "globals.h"

const string FONT_FILE = "alagard.ttf";
const int FONT_SIZE = 16;
const SDL_Color color = { 55, 55, 55, 55 };

Menu::Menu(Hero* hero) {
	setUp();
}

void Menu::draw() {
	drawMenuBackground();
	drawText();
	drawSelectionBox();
}

void Menu::setUp() {
	__super::setUp();
	index = 0;
}

void Menu::drawMenuBackground() {
	SDL_SetRenderDrawColor(Globals::renderer, color.r, color.g, color.b, color.a);
	SDL_Rect rect = {100, 100, Globals::ScreenWidth - 200, Globals::ScreenHeight - 200 };
	SDL_RenderFillRect(Globals::renderer, &rect);
}

void Menu::drawText() {
}

void Menu::drawSelectionBox() {
}
