#include "ui/menu/pauseMenu.h"

#include "hero.h"
#include "globals.h"

const string FONT_FILE = "alagard.ttf";
const int MENU_MAX_HEIGHT = Globals::ScreenHeight - (Globals::ScreenHeight / 4);
const int FONT_SIZE = 16;
const SDL_Color color = { 255, 255, 255, 255 };

PauseMenu::PauseMenu(Hero* hero) {
	this->hero = hero;
	setUp();
}

void PauseMenu::draw() {
	if (menuState == MenuState::Inactive) return;

	drawMenuBackground();
	drawText();
	drawSelectionBox();
}

void PauseMenu::setUp() {
	__super::setUp();
	menuState = MenuState::Inactive;
	currentPage = MenuPage::PAGE1;
}

void PauseMenu::drawMenuBackground() {
	SDL_Rect bgRect = { 50, Globals::ScreenHeight / 8, 150, MENU_MAX_HEIGHT };
	SDL_SetRenderDrawBlendMode(Globals::renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(Globals::renderer, 50, 50, 50, 120);
	SDL_RenderFillRect(Globals::renderer, &bgRect);
}

void PauseMenu::drawText() {
	textYOffset = 0;

	switch (currentPage) {
	case MenuPage::PAGE1:
		for (auto item : hero->inventory) {
			string text = item.second->name;
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

			renderTexture(fontTexture, Globals::renderer, 90 - textXOffset, (Globals::ScreenHeight / 8) + 2 + textYOffset);

			textYOffset += FONT_SIZE + 2;
		}
		break;
	case MenuPage::PAGE2:
		if (previousPage != currentPage) {
			menuItems.clear();
			menuItems.push_back("Resume");
			menuItems.push_back("Exit");
		}

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
			int textXOffset = (FONT_SIZE)*digits;

			renderTexture(fontTexture, Globals::renderer, 90 - textXOffset, (Globals::ScreenHeight / 8) + 2 + textYOffset);

			textYOffset += FONT_SIZE + 2;
		}
		break;
	}

	previousPage = currentPage;
}

void PauseMenu::drawSelectionBox() {
}
