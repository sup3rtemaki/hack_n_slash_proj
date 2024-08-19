#ifndef ESSENCE_COUNTER_UI
#define ESSENCE_COUNTER_UI

#include "ui.h"

class EssenceCounterUi : public Ui {
public:
	EssenceCounterUi(class Hero* hero);
	~EssenceCounterUi();

	virtual void draw() override;
	virtual void setUp() override;

private:
	string resPath;
	SDL_Texture* fontTexture;
	SDL_Texture* essenceCounterBarTexture;
	Hero* hero = nullptr;

	void drawBar();
	void drawEssenceQuantity();
};

#endif // !ESSENCE_COUNTER_UI
