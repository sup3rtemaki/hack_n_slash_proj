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
	bgImage = loadTexture(Ui::RES_PATH + Ui::HUD_TEXTURES_PATH + "main_menu_bg.png", Globals::renderer);
}

void Menu::drawMenuBackground() {
	//SDL_SetRenderDrawColor(Globals::renderer, color.r, color.g, color.b, color.a);
	//SDL_Rect rect = {100, 50, Globals::ScreenWidth - 200, Globals::ScreenHeight - 100 };
	//SDL_RenderFillRect(Globals::renderer, &rect);
	renderTexture(bgImage, Globals::renderer, 0, 0);
	menuItems[0] = "Start";
	menuItems[1] = "Exit";
}

void Menu::drawText() {
}

void Menu::drawSelectionBox() {
}
