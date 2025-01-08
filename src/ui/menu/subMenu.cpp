#include "ui/menu/subMenu.h"

#include "hero.h"
#include "item.h"
#include "globals.h"

const int MENU_HEIGHT = Globals::ScreenHeight / 4;
const int MENU_WIDTH = Globals::ScreenWidth / 4;
const int FONT_SIZE = 16;
const SDL_Color color = { 255, 255, 255, 255 };
int SUB_MENU_MAX_INDEX = 1;

SubMenu::SubMenu(Hero* hero) {
	__super::setUp();
	this->hero = hero;
	setUp();
}

void SubMenu::setUp() {
	menuState = MenuState::Inactive;
	selectionRect = new SDL_Rect();
	bgRect = new SDL_Rect();
	menuItems.push_back("Use");
	menuItems.push_back("Quick Inv");
	menuItems.push_back("Drop");
	SUB_MENU_MAX_INDEX = menuItems.size();
	index = 0;
}

void SubMenu::activateSubMenu(int x, int y) {
	menuState = MenuState::Active;
	xPos = x;
	yPos = y;
}

void SubMenu::draw() {
	if (menuState == MenuState::Inactive) return;

	drawMenuBackground();
	drawText();
	drawSelectionBox();
}

void SubMenu::drawMenuBackground() {
	const int bgRectWidth = Globals::ScreenWidth / 8;
	const int bgRectHeight = Globals::ScreenHeight / 6;
	*bgRect = { xPos, yPos, bgRectWidth, bgRectHeight };
	SDL_SetRenderDrawBlendMode(Globals::renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(Globals::renderer, 100, 100, 100, 150);
	SDL_RenderFillRect(Globals::renderer, bgRect);
}

void SubMenu::drawText() {
	textYOffset = 0;
	for (const auto& text : menuItems) {
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

		renderTexture(
			fontTexture,
			Globals::renderer,
			(bgRect->x + bgRect->w / 4) - textXOffset,
			bgRect->y + 2 + textYOffset);

		textYOffset += FONT_SIZE + 2;
	}
}

void SubMenu::drawSelectionBox() {
	const int yLinePos = yPos + (FONT_SIZE + 2) * (index + 1);
	SDL_SetRenderDrawColor(Globals::renderer, 200, 200, 200, 255);
	SDL_RenderDrawLine(
		Globals::renderer,
		xPos + 2,
		yLinePos,
		xPos + bgRect->w - 2,
		yLinePos);
}

void SubMenu::onIndexUp() {
	index--;

	if (index < 0) {
		index = 0;
	}
}

void SubMenu::onIndexDown() {
	index++;

	if (index >= SUB_MENU_MAX_INDEX) {
		index = SUB_MENU_MAX_INDEX - 1;
	}
}

void SubMenu::onIndexLeft() {
}

void SubMenu::onIndexRight() {
}
