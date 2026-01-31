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
	itemQuantityTexture = nullptr;
	lastQuantity = -1;
	lastItemId = -1;

	setUp();
}

QuickItemUi::~QuickItemUi() {
	if (quickItemFrame != nullptr) {
		SDL_DestroyTexture(quickItemFrame);
		quickItemFrame = nullptr;
	}

	if (itemQuantityTexture != nullptr) {
		SDL_DestroyTexture(itemQuantityTexture);
		itemQuantityTexture = nullptr;
	}

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

    int currentItemId = hero->quickAccessInventory[hero->quickAccessInventoryIndex];
    auto it = hero->inventory.find(currentItemId);
    if (it == hero->inventory.end()) return;

    int currentQuantity = it->second->quantity;

    // SO criar nova textura se quantidade mudou
    if (itemQuantityTexture == nullptr ||
        currentQuantity != lastQuantity ||
        currentItemId != lastItemId) {

        // Liberar textura antiga
        if (itemQuantityTexture != nullptr) {
            SDL_DestroyTexture(itemQuantityTexture);
            itemQuantityTexture = nullptr;
        }

        // Criar nova textura
        stringstream ss;
        ss << currentQuantity;
        itemQuantityTexture = renderText(
            ss.str(),
            Ui::RES_PATH + Ui::FONTS_PATH + FONT_FILE,
            color,
            ITEM_QUANTITY_FONT_SIZE,
            Globals::renderer
        );

        lastQuantity = currentQuantity;
        lastItemId = currentItemId;
    }

    // Renderizar textura cacheada
    renderTexture(itemQuantityTexture, Globals::renderer,
        ITEM_QUANTITY_FONT_X, ITEM_QUANTITY_FONT_Y);
}
