#include "ui/itemPickMessageUi.h"

const string FONT_FILE = "alagard.ttf";
const int FONT_X = 20;
const int FONT_Y = 280;
const int FONT_SIZE = 16;
const float SHOW_MESSAGE_TIME = 2.f;
const SDL_Color color = { 255, 255, 255, 255 };

ItemPickMessageUi::ItemPickMessageUi(Hero* hero) : hero(hero) {
	setUp();
}

ItemPickMessageUi::~ItemPickMessageUi() {
	hero = nullptr;
}

void ItemPickMessageUi::draw() {
	// TODO: Implementar mostrar mais de uma mensagem
	if (!hero->addedItemName.empty()) {
		timer = SHOW_MESSAGE_TIME;
		itemName = hero->addedItemName;
	}

	if (timer > 0) {
		stringstream message;
		message << "Looted " << hero->qtyItemsPicked << " " << itemName;
		const string& messageStr = message.str();

		if (fontTexture == nullptr ||
			prevMessage != messageStr) {
			fontTexture = renderText(
				messageStr,
				resPath + FONTS_PATH + FONT_FILE,
				color,
				FONT_SIZE,
				Globals::renderer
			);

			prevMessage = messageStr;
		}

		renderTexture(fontTexture, Globals::renderer, FONT_X, FONT_Y);

		timer -= TimeController::timeController.dT;
		hero->addedItemName = "";
	}

	SDL_DestroyTexture(fontTexture);
	fontTexture = nullptr;
}

void ItemPickMessageUi::setUp() {
	resPath = getResourcePath();
}

