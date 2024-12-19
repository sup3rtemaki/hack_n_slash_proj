#include "ui/menu/pauseMenu.h"

#include "hero.h"
#include "item.h"
#include "globals.h"

const string FONT_FILE = "Berylium.ttf";
const int MENU_MAX_HEIGHT = Globals::ScreenHeight - (Globals::ScreenHeight / 4);
const int FONT_SIZE = 25;
const SDL_Color color = { 255, 255, 255, 255 };
int MAX_INDEX = 1;

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
	previousPage = MenuPage::PAGE5;
	selectionRect = new SDL_Rect();

	selectionRect->w = 36;
	selectionRect->h = 36;

	if (hero == nullptr) return;

	for (auto item : hero->inventory) {
		inventory.push_back(item.second);
	}
}

void PauseMenu::drawMenuBackground() {
	const int bgRectX = Globals::ScreenWidth / 16;
	const int bgRectY = Globals::ScreenHeight / 16;
	const int bgRectWidth = Globals::ScreenWidth - Globals::ScreenWidth / 8;
	const int bgRectHeight = Globals::ScreenHeight - Globals::ScreenHeight / 8;
	SDL_Rect bgRect = { bgRectX, bgRectY, bgRectWidth, bgRectHeight };
	SDL_SetRenderDrawBlendMode(Globals::renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(Globals::renderer, 50, 50, 50, 120);
	SDL_RenderFillRect(Globals::renderer, &bgRect);
}

void PauseMenu::drawText() {
	textYOffset = 0;

	switch (currentPage) {
	case MenuPage::PAGE1:
		drawPage1();
		break;
	case MenuPage::PAGE2:
		drawPage2();
		break;
	}

	previousPage = currentPage;
}

void PauseMenu::drawSelectionBox() {
	selectionRect->x = indexXPosition;
	selectionRect->y = indexYPosition;

	SDL_SetRenderDrawColor(Globals::renderer, 200, 200, 200, 255);
	SDL_RenderDrawRect(Globals::renderer, selectionRect);

	//const int yLinePos = (Globals::ScreenHeight / 8) + (FONT_SIZE + 2) * (index + 1);
	//SDL_SetRenderDrawColor(Globals::renderer, 200, 200, 200, 255);
	//SDL_RenderDrawLine(
	//	Globals::renderer,
	//	90,
	//	yLinePos,
	//	150,
	//	yLinePos);
}

void PauseMenu::drawPage1() {
	if (previousPage != currentPage) {
		MAX_INDEX = 2;
		supVisibleItemsLimit = MAX_INDEX;
		infVisibleItemsLimit = 0;
		menuItems.clear();
		menuItems.push_back("Resume");
		menuItems.push_back("Options");
		menuItems.push_back("Credits");
		menuItems.push_back("Exit");
		index = 0;
		currentPage = MenuPage::PAGE1;
	}

	vector<string> menuItemsToShow;
	for (int i = infVisibleItemsLimit; i < supVisibleItemsLimit; i++) {
		menuItemsToShow.push_back(menuItems[i]);
	}

	for (auto text : menuItemsToShow) {
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
}

void PauseMenu::drawPage2() {
	// Atualiza inventario
	if (inventory.size() != hero->inventory.size()) {
		inventory.clear();
		for (auto item : hero->inventory) {
			inventory.push_back(item.second);
		}
	}

	// Atualiza menuItems caso mude de pagina
	if (previousPage != currentPage) {
		menuItems.clear();
		itemsImages.clear();
		for (auto item : inventory) {
			//menuItems.push_back(item->name);
			itemsImages.push_back(item->image);
		}
		// MAX_INDEX = menuItems.size() - 1;
		MAX_INDEX = 32;
		index = 0;
		currentPage = MenuPage::PAGE2;
	}

	int x = (Globals::ScreenWidth / 6);
	int y = (Globals::ScreenHeight / 8) + 5;
	for (auto image : itemsImages) {
		renderTexture(
			image,
			Globals::renderer,
			x,
			y
		);
		x += 50;
	}

	// Desenha nomes dos itens
	//for (const string& itemName : menuItems) {
	//	fontTexture = renderText(
	//		itemName,
	//		Ui::RES_PATH + Ui::FONTS_PATH + FONT_FILE,
	//		color,
	//		FONT_SIZE,
	//		Globals::renderer
	//	);
	//	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	//	int digits;
	//	(int)itemName.size() > 0 ?
	//		digits = int(log10((int)itemName.size()) + 1) :
	//		digits = 1;
	//	int textXOffset = (FONT_SIZE)*digits;

	//	renderTexture(fontTexture, Globals::renderer, 95 - textXOffset, (Globals::ScreenHeight / 8) + 2 + textYOffset);

	//	textYOffset += FONT_SIZE + 2;
	//}

	// Desenha item selecionado e texto de descrição do item
	//for (auto item : inventory) {
	//	if (item->name == menuItems[index]) {
	//		renderTexture(
	//			item->image,
	//			Globals::renderer,
	//			(Globals::ScreenWidth / 2) + 5,
	//			(Globals::ScreenHeight / 8) + 5
	//		);

	//		TTF_Font* font = nullptr;
	//		font = TTF_OpenFont((Ui::RES_PATH + Ui::FONTS_PATH + FONT_FILE).c_str(), FONT_SIZE);

	//		auto textSurf = TTF_RenderText_Blended_Wrapped(font, item->description.c_str(), color, 200);
	//		SDL_Texture* itemDescTexture = SDL_CreateTextureFromSurface(Globals::renderer, textSurf);

	//		renderTexture(
	//			itemDescTexture,
	//			Globals::renderer,
	//			(Globals::ScreenWidth / 2) + 5 + 32 + 2,
	//			(Globals::ScreenHeight / 8) + 5
	//		);
	//	}
	//}
}

void PauseMenu::drawPage3() {
}

void PauseMenu::drawPage4() {
}

void PauseMenu::drawPage5() {
}

SDL_Point PauseMenu::calculateRectSelectionBoxPosition() {
	SDL_Point position;

	position.x = (Globals::ScreenWidth / 6) * ((index % 3) + 1);
	position.y = (Globals::ScreenHeight / 8) * ((index / 3) + 1);

	return position;
}

void PauseMenu::onIndexUp() {
	switch (currentPage) {
	case MenuPage::PAGE1:
		index--;

		if (index < 0) {
			index = 0;

			if (infVisibleItemsLimit > 0) {
				supVisibleItemsLimit--;
				infVisibleItemsLimit--;
			}
		}
		break;
	case MenuPage::PAGE2:
		index -= 3;

		if (index < 0) {
			index = 0;

			if (infVisibleItemsLimit > 0) {
				supVisibleItemsLimit--;
				infVisibleItemsLimit--;
			}
		}
		break;
	default:
		break;
	}
}

void PauseMenu::onIndexDown() {
	switch (currentPage) {
	case MenuPage::PAGE1:
		index++;

		if (index >= MAX_INDEX) {
			index = MAX_INDEX - 1;

			if (menuItems.size() > supVisibleItemsLimit) {
				supVisibleItemsLimit++;
				infVisibleItemsLimit++;
			}
		}
		break;
	case MenuPage::PAGE2:
		index += 3;

		if (index >= MAX_INDEX) {
			index = MAX_INDEX - 1;

			if (menuItems.size() > supVisibleItemsLimit) {
				supVisibleItemsLimit++;
				infVisibleItemsLimit++;
			}
		}
		break;
	default:
		break;
	}
}

void PauseMenu::onIndexLeft() {
	switch (currentPage) {
	case MenuPage::PAGE1:
		break;
	case MenuPage::PAGE2:
		index--;

		if (index < 0) {
			index = 0;

			if (infVisibleItemsLimit > 0) {
				supVisibleItemsLimit--;
				infVisibleItemsLimit--;
			}
		}
		break;
	default:
		break;
	}
}

void PauseMenu::onIndexRight() {
	switch (currentPage) {
	case MenuPage::PAGE1:
		break;
	case MenuPage::PAGE2:
		index++;

		if (index >= MAX_INDEX) {
			index = MAX_INDEX - 1;

			if (menuItems.size() > supVisibleItemsLimit) {
				supVisibleItemsLimit++;
				infVisibleItemsLimit++;
			}
		}
		break;
	default:
		break;
	}
}
