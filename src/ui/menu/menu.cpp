#include "ui/menu/menu.h"

#include "hero.h"
#include "globals.h"

const string FONT_FILE = "alagard.ttf";
const int FONT_SIZE = 32;
const int FONT_X = (Globals::ScreenWidth / 2);
const int FONT_Y = (Globals::ScreenHeight / 2);
const SDL_Color color = { 255, 255, 255, 255 };

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
	textYOffset = 0;
	menuItems.push_back("Start");
	menuItems.push_back("Exit");
	bgImage = loadTexture(Ui::RES_PATH + Ui::HUD_TEXTURES_PATH + "main_menu_bg.png", Globals::renderer);
}

void Menu::drawMenuBackground() {
	renderTexture(bgImage, Globals::renderer, 0, 0);
}

void Menu::drawText() {
	if (menuItems.empty()) return;

	textYOffset = 0;

	for (auto text : menuItems) {
		fontTexture = renderText(
			text,
			Ui::RES_PATH + Ui::FONTS_PATH + FONT_FILE,
			color,
			FONT_SIZE,
			Globals::renderer
		);

		int digits;
		(int)text.size() > 0 ?
			digits = int(log10((int)text.size()) + 1) :
			digits = 1;
		int xOffsetAlign = (FONT_SIZE) * digits;

		renderTexture(fontTexture, Globals::renderer, FONT_X - xOffsetAlign, FONT_Y + textYOffset);

		textYOffset += FONT_SIZE + 5;
	}
}

void Menu::drawSelectionBox() {
}
