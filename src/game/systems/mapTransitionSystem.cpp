#include "mapTransitionSystem.h"

MapTransitionSystem::MapTransitionSystem(SDL_Texture* fadeImage)
	: fadeImage(fadeImage) {
}

void MapTransitionSystem::startFadeIn() {
	if (isFading) {
		return;
	}

	isFading = true;
	fadeIn = true;
	fadeOut = false;
	mapChangeCallbackCalled = false;
	alpha = 0;
	alphaCalc = 0.0f;
	SDL_SetTextureAlphaMod(fadeImage, alpha);
}

void MapTransitionSystem::startFadeOut() {
	isFading = true;
	fadeIn = false;
	fadeOut = true;
	alpha = 255;
	alphaCalc = 255.0f;
	SDL_SetTextureAlphaMod(fadeImage, alpha);
}

void MapTransitionSystem::update() {
	if (!isFading) {
		SDL_SetTextureAlphaMod(fadeImage, 0);
		return;
	}

	if (fadeIn) {
		updateFadeIn();
	}
	else if (fadeOut) {
		updateFadeOut();
	}
}

bool MapTransitionSystem::isTransitioning() const {
	return isFading;
}

int MapTransitionSystem::getAlpha() const {
	return alpha;
}

void MapTransitionSystem::setMapChangeCallback(std::function<void()> callback) {
	mapChangeCallback = callback;
}

void MapTransitionSystem::updateFadeIn() {
	alphaCalc += 15.0f;
	alpha = static_cast<int>(alphaCalc);
	if (alpha > 255) {
		alpha = 255;
	}
	SDL_SetTextureAlphaMod(fadeImage, alpha);

	if (alpha >= 255 && !mapChangeCallbackCalled) {
		mapChangeCallbackCalled = true;
		if (mapChangeCallback) {
			mapChangeCallback();
		}
	}
}

void MapTransitionSystem::updateFadeOut() {
	alphaCalc -= 10.0f;
	alpha = static_cast<int>(alphaCalc);
	if (alpha < 0) {
		alpha = 0;
	}
	SDL_SetTextureAlphaMod(fadeImage, alpha);

	if (alpha == 0) {
		isFading = false;
		fadeOut = false;
		mapChangeCallbackCalled = false;
	}
}
