#include "ui/menu/pauseMenu.h"

#include "ui/menu/subMenu.h"
#include "hero.h"
#include "item.h"
#include "globals.h"

const string& PAUSE_MENU_ITEMS_BG_FILE = "\\Assets\\Textures\\HUD\\pause_menu_items_bg.png";
const string& PAUSE_ARROW_LEFT_FILE = "\\Assets\\Textures\\HUD\\arrow_left.png";
const string& PAUSE_ARROW_RIGHT_FILE = "\\Assets\\Textures\\HUD\\arrow_right.png";
const int MENU_MAX_HEIGHT = Globals::ScreenHeight - (Globals::ScreenHeight / 4);
const int FONT_SIZE = 25;
const int ITEMS_IMAGES_GRID_X_POSITION = Globals::ScreenWidth / 6;
const int ITEMS_IMAGES_GRID_Y_POSITION = Globals::ScreenHeight / 8;
const int ITEMS_IMAGES_X_OFFSET = 50;
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

	if (subMenu->menuState == MenuState::Active) {
		subMenu->draw();
	}
}

void PauseMenu::drawPageInitialCheck() {
	switch (currentPage) {
	case MenuPage::PAGE1:
		// Atualiza menuItems caso mude de pagina
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
		break;
	case MenuPage::PAGE2:
		// Atualiza inventarios
		if (inventory.size() != hero->inventory.size()) {
			inventory.clear();
			for (auto item : hero->inventory) {
				inventory.push_back(item.second);
			}
		}

		// Atualiza menuItems caso mude de pagina
		if (previousPage != currentPage) {
			menuItems.clear();
			MAX_INDEX = inventory.size();
			index = 0;
			currentPage = MenuPage::PAGE2;
		}
		break;
	}
}

void PauseMenu::drawInventoryItems() {
	// Desenha a imagem de fundo dos items
	renderTexture(
		itemsBg,
		Globals::renderer,
		ITEMS_IMAGES_GRID_X_POSITION,
		ITEMS_IMAGES_GRID_Y_POSITION
	);

	// Desenha imagens dos itens no inventario
	int textureXPos = ITEMS_IMAGES_GRID_X_POSITION;
	int textureYPos = ITEMS_IMAGES_GRID_Y_POSITION;
	int textureXPosReset = 0;
	int textureYPosOffset = 0;
	for (auto item : inventory) {
		renderTexture(
			item->image,
			Globals::renderer,
			textureXPos,
			textureYPos
		);
		textureXPos += ITEMS_IMAGES_X_OFFSET;
		textureYPosOffset++;
		textureXPosReset++;

		if (textureXPosReset >= 3) {
			textureXPosReset = 0;
			textureXPos = ITEMS_IMAGES_GRID_X_POSITION;
		}

		if (textureYPosOffset > 0 && textureYPosOffset % 3 == 0) {
			textureYPos += (Globals::ScreenHeight / 8);
		}
	}
}

void PauseMenu::drawSelectedItemNameAndDescription() {
	// Desenha nome do item selecionado
	const string& itemName = inventory.at(index)->name;
	fontTexture = renderText(
		itemName,
		Ui::RES_PATH + Ui::FONTS_PATH + FONT_FILE,
		color,
		FONT_SIZE,
		Globals::renderer
	);

	int digits;
	(int)itemName.size() > 0 ?
		digits = int(log10((int)itemName.size()) + 1) :
		digits = 1;
	int textXOffset = (FONT_SIZE)*digits;

	renderTexture(fontTexture,
		Globals::renderer,
		((Globals::ScreenWidth / 2) + ITEMS_IMAGES_GRID_X_POSITION) - textXOffset,
		(Globals::ScreenHeight / 8));

	// Desenha descrição do item selecionado
	const string& itemDesc = inventory.at(index)->description;

	TTF_Font* font = nullptr;
	font = TTF_OpenFont((Ui::RES_PATH + Ui::FONTS_PATH + FONT_FILE).c_str(), (int)(FONT_SIZE / 1.5));
	auto textSurf = TTF_RenderText_Blended_Wrapped(font, itemDesc.c_str(), color, 200);
	fontTexture = SDL_CreateTextureFromSurface(Globals::renderer, textSurf);

	(int)itemDesc.size() > 0 ?
		digits = int(log10((int)itemDesc.size()) + 1) :
		digits = 1;
	textXOffset = (FONT_SIZE)*digits;

	renderTexture(fontTexture,
		Globals::renderer,
		((Globals::ScreenWidth / 2) + ITEMS_IMAGES_GRID_X_POSITION) - textXOffset,
		(Globals::ScreenHeight / 8) + (FONT_SIZE * 1.2));
}

void PauseMenu::drawQuickInventory() {
	// Desenha imagens dos itens no inventario
	int textureXPos = ((Globals::ScreenWidth / 2) + ITEMS_IMAGES_GRID_X_POSITION - 32);
	int textureYPos = 270;
	int textureXPosReset = 0;
	int textureYPosOffset = 0;
	for (auto item : inventory) {
		if (
			std::find(
				hero->quickAccessInventory.begin(),
				hero->quickAccessInventory.end(),
				item->id) ==
			hero->quickAccessInventory.end()) {
			continue;
		}

		renderTexture(
			item->image,
			Globals::renderer,
			textureXPos,
			textureYPos
		);
		textureXPos += ITEMS_IMAGES_X_OFFSET - 10;
		textureYPosOffset++;
		textureXPosReset++;

		if (textureXPosReset >= 5) {
			textureXPosReset = 0;
			textureXPos = ((Globals::ScreenWidth / 2) + ITEMS_IMAGES_GRID_X_POSITION);;
		}

		if (textureYPosOffset > 0 && textureYPosOffset % 5 == 0) {
			textureYPos += (Globals::ScreenHeight / 8);
		}
	}
}

void PauseMenu::setUp() {
	__super::setUp();
	subMenu = new SubMenu(hero);
	menuState = MenuState::Inactive;
	currentPage = MenuPage::PAGE1;
	previousPage = MenuPage::PAGE5;

	selectionRect = new SDL_Rect();
	selectionRect->w = 36;
	selectionRect->h = 36;

	bgRect = new SDL_Rect();

	itemsBg = loadTexture(
		Ui::RES_PATH + PAUSE_MENU_ITEMS_BG_FILE,
		Globals::renderer
	);
	leftArrowTexture = loadTexture(
		Ui::RES_PATH + PAUSE_ARROW_LEFT_FILE,
		Globals::renderer
	);
	rightArrowTexture = loadTexture(
		Ui::RES_PATH + PAUSE_ARROW_RIGHT_FILE,
		Globals::renderer
	);

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
	*bgRect = { bgRectX, bgRectY, bgRectWidth, bgRectHeight };
	SDL_SetRenderDrawBlendMode(Globals::renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(Globals::renderer, 50, 50, 50, 120);
	SDL_RenderFillRect(Globals::renderer, bgRect);

	switch (currentPage) {
	case MenuPage::PAGE1:
		renderTexture(
			rightArrowTexture,
			Globals::renderer,
			bgRectX + bgRectWidth - 5,
			ITEMS_IMAGES_GRID_Y_POSITION + 100
		);
		break;
	case MenuPage::PAGE2:
		renderTexture(
			leftArrowTexture,
			Globals::renderer,
			bgRectX - 5,
			ITEMS_IMAGES_GRID_Y_POSITION + 100
		);
		break;
	}
}

void PauseMenu::drawText() {
	if (menuState == MenuState::Inactive) return;

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
	switch (currentPage) {
	case MenuPage::PAGE1: {
		const int yLinePos = (Globals::ScreenHeight / 8) + (FONT_SIZE + 2) * (index + 1);
		SDL_SetRenderDrawColor(Globals::renderer, 200, 200, 200, 255);
		SDL_RenderDrawLine(
			Globals::renderer,
			90,
			yLinePos,
			150,
			yLinePos);
		break;
	}
	case MenuPage::PAGE2:
		SDL_Point selectionRectPos = calculateRectSelectionBoxPosition();
		selectionRect->x = selectionRectPos.x;
		selectionRect->y = selectionRectPos.y;
		SDL_SetRenderDrawColor(Globals::renderer, 200, 200, 200, 255);
		SDL_RenderDrawRect(Globals::renderer, selectionRect);
		break;
	}
}

void PauseMenu::drawPage1() {
	drawPageInitialCheck();

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
	drawPageInitialCheck();
	drawInventoryItems();
	drawSelectedItemNameAndDescription();
	drawQuickInventory();
}

void PauseMenu::drawPage3() {
}

void PauseMenu::drawPage4() {
}

void PauseMenu::drawPage5() {
}

SDL_Point PauseMenu::calculateRectSelectionBoxPosition() {
	SDL_Point position;
	int xMultiplier;
	int yMultiplier;

	xMultiplier = index % 3;
	yMultiplier = (index / 3);

	position.x = (Globals::ScreenWidth / 6) + (ITEMS_IMAGES_X_OFFSET * xMultiplier) - 2;
	position.y = ((Globals::ScreenHeight / 8) * yMultiplier + 45) - 2;

	return position;
}

void PauseMenu::onIndexUp() {
	switch (menuState) {
	case MenuState::Active:
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

				//if (infVisibleItemsLimit > 0) {
				//	supVisibleItemsLimit--;
				//	infVisibleItemsLimit--;
				//}
			}
			break;
		default:
			break;
		}
		break;
	case MenuState::Background:
		subMenu->onIndexUp();
		break;
	}
}

void PauseMenu::onIndexDown() {
	switch (menuState) {
	case MenuState::Active:
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

				//if (menuItems.size() > supVisibleItemsLimit) {
				//	supVisibleItemsLimit++;
				//	infVisibleItemsLimit++;
				//}
			}
			break;
		default:
			break;
		}
		break;
	case MenuState::Background:
		subMenu->onIndexDown();
		break;
	}
}

void PauseMenu::onIndexLeft() {
	if (menuState == MenuState::Active) {
		switch (currentPage) {
		case MenuPage::PAGE1:
			break;
		case MenuPage::PAGE2:
			index--;

			if (index < 0) {
				index = 0;

				//if (infVisibleItemsLimit > 0) {
				//	supVisibleItemsLimit--;
				//	infVisibleItemsLimit--;
				//}
			}
			break;
		default:
			break;
		}
	}
}

void PauseMenu::onIndexRight() {
	if (menuState == MenuState::Active) {
		switch (currentPage) {
		case MenuPage::PAGE1:
			break;
		case MenuPage::PAGE2:
			index++;

			if (index >= MAX_INDEX) {
				index = MAX_INDEX - 1;

				//if (menuItems.size() > supVisibleItemsLimit) {
				//	supVisibleItemsLimit++;
				//	infVisibleItemsLimit++;
				//}
			}
			break;
		default:
			break;
		}
	}
}

void PauseMenu::showSubMenu() {
	subMenu->activateSubMenu(selectionRect->x + 32, selectionRect->y);
	menuState = MenuState::Background;
}

void PauseMenu::hideSubMenu() {
	subMenu->menuState = MenuState::Inactive;
	menuState = MenuState::Active;
}
