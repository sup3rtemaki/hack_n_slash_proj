#include "ui/quickItemUi.h"

#include "globals.h"

const string& ITEM_FRAME_FILE = "itemFrame.png";
const string& ITEM_FRAME_FOLDER = "\\Assets\\Textures\\HUD\\";
const string& FONT_FILE = "alagard.ttf";
const int ITEM_FRAME_X = 24;
const int ITEM_FRAME_Y = 304;
const int ITEM_X = 20;
const int ITEM_Y = 300;
const int ITEM_QUANTITY_FONT_X = 28;
const int ITEM_QUANTITY_FONT_Y = 324;
const int ITEM_QUANTITY_FONT_SIZE = 16;
const SDL_Color color = { 255, 255, 255, 255 };

QuickItemUi::QuickItemUi(Hero* hero) : hero(hero) {
	setUp();
}

QuickItemUi::~QuickItemUi() {
	hero = nullptr;
}

void QuickItemUi::draw() {
	if (hero == nullptr) return;

	drawItemFrame();
	drawCurrentItem();
	drawItemQuantity();
}

void QuickItemUi::setUp() {
	__super::setUp();
	quickItemFrame =
		loadTexture(Ui::RES_PATH + ITEM_FRAME_FOLDER + ITEM_FRAME_FILE, Globals::renderer);
}

void QuickItemUi::drawItemFrame() {
	renderTexture(quickItemFrame, Globals::renderer, ITEM_X, ITEM_Y);
}

void QuickItemUi::drawCurrentItem() {
	if (hero->quickAccessInventory.empty() || 
		hero->quickAccessInventory[hero->quickAccessInventoryIndex] < 0) return;

	renderTexture(
		hero->inventory.find(hero->quickAccessInventory[hero->quickAccessInventoryIndex])->second->image,
		Globals::renderer,
		ITEM_FRAME_X, ITEM_FRAME_Y
	);
}

void QuickItemUi::drawItemQuantity() {
	if (hero->quickAccessInventory.empty() ||
		hero->quickAccessInventory[hero->quickAccessInventoryIndex] < 0) return;

	stringstream ss;
	ss << hero->inventory.find(hero->quickAccessInventory[hero->quickAccessInventoryIndex])->second->quantity;
	SDL_Texture* currentItemQuantity = renderText(
		ss.str(),
		Ui::RES_PATH + Ui::FONTS_PATH + FONT_FILE,
		color, 
		ITEM_QUANTITY_FONT_SIZE,
		Globals::renderer
	);
	renderTexture(currentItemQuantity, Globals::renderer, ITEM_QUANTITY_FONT_X, ITEM_QUANTITY_FONT_Y);
}
