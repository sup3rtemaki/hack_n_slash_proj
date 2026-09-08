#include "ui/essenceCounterUi.h"
#include "resourceConfig.h"

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
	fontTexture = nullptr;
	lastEssence = -1;

	setUp();
}

EssenceCounterUi::~EssenceCounterUi() {
	if (essenceCounterBarTexture != nullptr) {
		SDL_DestroyTexture(essenceCounterBarTexture);
		essenceCounterBarTexture = nullptr;
	}

	if (fontTexture != nullptr) {
		SDL_DestroyTexture(fontTexture);
		fontTexture = nullptr;
	}

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
    essenceCounterBarTexture = nullptr;
}

void EssenceCounterUi::setRenderer(SDL_Renderer* rendererContext) {
    Ui::setRenderer(rendererContext);
    if (renderer == nullptr || essenceCounterBarTexture != nullptr) return;

    essenceCounterBarTexture = loadTexture(
        Ui::RES_PATH + ESSENCE_COUNTER_BAR_FOLDER + ESSENCE_COUNTER_BAR_FILE,
        renderer
    );
}

void EssenceCounterUi::drawBar() {
    if (renderer == nullptr) return;
    renderTexture(essenceCounterBarTexture, renderer, FONT_X + BAR_X_OFFSET, FONT_Y + BAR_Y_OFFSET);
}

void EssenceCounterUi::drawEssenceQuantity() {
    if (renderer == nullptr) return;

    int currentEssence = hero->essence;

    // SO criar nova textura se essence mudou
    if (fontTexture == nullptr || currentEssence != lastEssence) {

        // Liberar textura antiga
        if (fontTexture != nullptr) {
            SDL_DestroyTexture(fontTexture);
            fontTexture = nullptr;
        }

        // Criar nova textura
        stringstream message;
        message << std::to_string(currentEssence);
        fontTexture = renderText(
            message.str(),
            Ui::RES_PATH + ResourcePaths::FONTS + FONT_FILE,
            color,
            FONT_SIZE,
			renderer
        );

        lastEssence = currentEssence;
    }

    // Renderizar textura cacheada
    int digits;
    currentEssence > 0 ?
        digits = int(log10(currentEssence) + 1) :
        digits = 1;
    int xOffsetAlign = (FONT_SIZE / 2) * digits;
	renderTexture(fontTexture, renderer, FONT_X - xOffsetAlign, FONT_Y);
}