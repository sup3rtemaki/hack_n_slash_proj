#include "ui/essenceCounterUi.h"

#include "hero.h"
#include <iomanip>

const string& ESSENCE_COUNTER_BAR_FILE = "essence_counter_bar.png";
const string& ESSENCE_COUNTER_BAR_FOLDER = "\\Assets\\Textures\\HUD\\";
const string FONT_FILE = "alagard.ttf";
const int FONT_X = 620;
const int FONT_Y = 325;
const int BAR_X_OFFSET = -106;
const int BAR_Y_OFFSET = -2;
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

	drawBar();
	drawEssenceQuantity();
}

void EssenceCounterUi::setUp() {
	__super::setUp();
	essenceCounterBarTexture =
		loadTexture(Ui::RES_PATH + ESSENCE_COUNTER_BAR_FOLDER + ESSENCE_COUNTER_BAR_FILE, Globals::renderer);
}

void EssenceCounterUi::drawBar() {
	renderTexture(essenceCounterBarTexture, Globals::renderer, FONT_X + BAR_X_OFFSET, FONT_Y + BAR_Y_OFFSET);
}

void EssenceCounterUi::drawEssenceQuantity() {
	stringstream message;
	message << std::to_string(hero->essence);

	fontTexture = renderText(
		message.str(),
		Ui::RES_PATH + Ui::FONTS_PATH + FONT_FILE,
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
}
