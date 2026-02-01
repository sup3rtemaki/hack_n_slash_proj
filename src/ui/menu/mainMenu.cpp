#include "ui/menu/mainMenu.h"

#include "hero.h"
#include "globals.h"

const int FONT_SIZE = 32;
const int FONT_X = (Globals::ScreenWidth / 2);
const int FONT_Y = (Globals::ScreenHeight / 2);
const SDL_Color color = { 255, 255, 255, 255 };
const int MAX_INDEX = 1;

MainMenu::MainMenu() {
	setUp();
}

void MainMenu::setUp() {
	__super::setUp();
	FONT_FILE = "alagard.ttf";
	menuState = MenuState::Active;
	index = 0;
	textYOffset = 0;
	menuItems.push_back("Start");
	menuItems.push_back("Exit");
	bgImage = loadTexture(Ui::RES_PATH + Ui::HUD_TEXTURES_PATH + "main_menu_bg.png", Globals::renderer);
}

void MainMenu::draw() {
	if (menuState == MenuState::Inactive) return;

	if (index > MAX_INDEX) {
		index = 0;
	}

	if (index < 0) {
		index = MAX_INDEX;
	}

	drawMenuBackground();
	drawText();
	drawSelectionBox();
}

void MainMenu::drawMenuBackground() {
	renderTexture(bgImage, Globals::renderer, 0, 0);
}

void MainMenu::drawText() {
	if (menuItems.empty()) return;

	textYOffset = 0;

	for (auto text : menuItems) {
		if (fontTexture != nullptr) {
			SDL_DestroyTexture(fontTexture);
			fontTexture = nullptr;
		}

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
		int textXOffset = (FONT_SIZE)*digits;

		renderTexture(fontTexture, Globals::renderer, FONT_X - textXOffset, FONT_Y + textYOffset);

		textYOffset += FONT_SIZE + 5;
	}
}

void MainMenu::drawSelectionBox() {
	SDL_Rect selectionRect = { 0, 0, 0, 0 };
	switch (index)
	{
	case 0:
	default:
		selectionRect.x = FONT_X - 35;
		selectionRect.y = FONT_Y - 5;
		selectionRect.w = 80;
		selectionRect.h = FONT_SIZE + 4;
		break;
	case 1:
		selectionRect.x = FONT_X - 35;
		selectionRect.y = FONT_Y - 5 + FONT_SIZE + 5;
		selectionRect.w = 80;
		selectionRect.h = FONT_SIZE + 4;
		break;
	}

	SDL_RenderDrawRect(Globals::renderer, &selectionRect);
}

void MainMenu::onIndexUp()
{
}

void MainMenu::onIndexDown()
{
}

void MainMenu::onIndexLeft()
{
}

void MainMenu::onIndexRight()
{
}
