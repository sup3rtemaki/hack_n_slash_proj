#include "ui/quickItemUi.h"

#include "globals.h"

const string& ITEM_FRAME_FILE = "itemFrame.png";
const string& ITEM_QUANTITY_FONT_FILE = "vermin_vibes_1989.ttf";
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

void QuickItemUi::drawItemFrame() {
	renderTexture(quickItemFrame, Globals::renderer, ITEM_X, ITEM_Y);
}

void QuickItemUi::drawCurrentItem() {
	renderTexture(
		hero->inventory.find(hero->inventoryIndex)->second->image,
		Globals::renderer,
		ITEM_FRAME_X, ITEM_FRAME_Y
	);
}

void QuickItemUi::drawItemQuantity() {
	stringstream ss;
	ss << hero->inventory.find(hero->inventoryIndex)->second->quantity;
	SDL_Texture* currentItemQuantity = renderText(
		ss.str(),
		resPath + ITEM_QUANTITY_FONT_FILE,
		color, 
		ITEM_QUANTITY_FONT_SIZE,
		Globals::renderer
	);
	renderTexture(currentItemQuantity, Globals::renderer, ITEM_QUANTITY_FONT_X, ITEM_QUANTITY_FONT_Y);
}

void QuickItemUi::draw() {
	drawItemFrame();
	drawCurrentItem();
	drawItemQuantity();
}

void QuickItemUi::setUp() {
	resPath = getResourcePath();
	quickItemFrame = loadTexture(resPath + ITEM_FRAME_FILE, Globals::renderer);
}
