#include "ui/essenceCounterUi.h"

#include "hero.h"
#include <iomanip>

const string FONT_FILE = "alagard.ttf";
const int FONT_X = 550;
const int FONT_Y = 330;
const int FONT_SIZE = 16;
const SDL_Color color = { 255, 255, 255, 255 };

EssenceCounterUi::EssenceCounterUi(Hero* hero) : hero{ hero } {
	setUp();
}

EssenceCounterUi::~EssenceCounterUi() {
	hero = nullptr;
	fontTexture = nullptr;
}

void EssenceCounterUi::draw() {
	if (hero == nullptr) return;

	stringstream message;
	message << std::to_string(hero->essence);

	fontTexture = renderText(
		message.str(),
		Ui::RES_PATH + FONTS_PATH + FONT_FILE,
		color,
		FONT_SIZE,
		Globals::renderer
	);

	int digits;
	hero->essence > 0 ?
		digits = int(log10(hero->essence) + 1) :
		digits = 1;
	int xOffsetAlign = (FONT_SIZE / 2) * digits;

	renderTexture(fontTexture, Globals::renderer, FONT_X - xOffsetAlign, FONT_Y);
	SDL_RenderDrawPoint(Globals::renderer, FONT_X, FONT_Y);
}

void EssenceCounterUi::setUp() {
	__super::setUp();
}
