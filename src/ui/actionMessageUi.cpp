#include "ui/actionMessageUi.h"

#include "globals.h"
#include "timeController.h"

#include <sstream>

const string FONT_FILE = "alagard.ttf";
const string MESSAGE_BAR_TEXTURE_FILE = "message_bar_tex.png";
const int SCREEN_CENTER_X = Globals::ScreenWidth / 2;
const int FONT_Y = 250;
const int FONT_SIZE = 16;
const float SHOW_MESSAGE_TIME = 0.1f;
const SDL_Color color = { 255, 255, 255, 255 };

ActionMessageUi::ActionMessageUi() {
	setUp();
}

void ActionMessageUi::draw() {
	if (timer > 0) {
		if (fontTexture == nullptr) {		
            fontX = SCREEN_CENTER_X - ((message.length() * FONT_SIZE) / 4);

			fontTexture = renderText(
				message,
				resPath + FONTS_PATH + FONT_FILE,
				color,
				FONT_SIZE,
				Globals::renderer
			);
		}

		SDL_Rect backgroundMessageRect;
        backgroundMessageRect.x = fontX - (FONT_SIZE * 2);
        backgroundMessageRect.y = FONT_Y;
        backgroundMessageRect.w = (message.length() * FONT_SIZE),
        backgroundMessageRect.h = FONT_SIZE;        

        backgroundBarTexture = loadTexture(resPath + Ui::HUD_TEXTURES_PATH + MESSAGE_BAR_TEXTURE_FILE, Globals::renderer);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        
        SDL_RenderCopy(Globals::renderer, backgroundBarTexture, NULL, &backgroundMessageRect);

		renderTexture(fontTexture, Globals::renderer, fontX, FONT_Y);

		timer -= TimeController::timeController.dT;
		return;
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	backgroundBarTexture = nullptr;
	fontTexture = nullptr;
	message.clear();
}

void ActionMessageUi::setUp() {
	resPath = getResourcePath();
}

void ActionMessageUi::setMessage(string message) {
	this->message = message;
	timer = SHOW_MESSAGE_TIME;
}
