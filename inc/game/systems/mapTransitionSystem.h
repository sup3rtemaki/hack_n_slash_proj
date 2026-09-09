#ifndef MAPTRANSITIONSYSTEM_H
#define MAPTRANSITIONSYSTEM_H

#include <SDL.h>
#include <functional>

class MapTransitionSystem {
public:
	explicit MapTransitionSystem(SDL_Texture* fadeImage);

	void startFadeIn();
	void startFadeOut();
	void update();

	bool isTransitioning() const;
	int getAlpha() const;

	void setMapChangeCallback(std::function<void()> callback);

private:
	SDL_Texture* fadeImage;
	bool isFading = false;
	bool fadeIn = false;
	bool fadeOut = false;
	bool mapChangeCallbackCalled = false;
	int alpha = 0;
	float alphaCalc = 0.0f;
	std::function<void()> mapChangeCallback;

	void updateFadeIn();
	void updateFadeOut();
};

#endif // MAPTRANSITIONSYSTEM_H
