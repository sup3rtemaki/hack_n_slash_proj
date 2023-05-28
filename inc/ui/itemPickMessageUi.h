#ifndef ITEMPICKMESSAGEUI
#define ITEMPICKMESSAGEUI

#include "ui.h"

#include "hero.h"

#include <string>

class ItemPickMessageUi : public Ui {
public:
	ItemPickMessageUi(Hero* hero);
	~ItemPickMessageUi();

	virtual void draw() override;
	virtual void setUp() override;

private:
	string resPath;
	string itemName;
	SDL_Texture* fontTexture;
	Hero* hero = nullptr;
	float timer;

};

#endif // !ITEMPICKMESSAGEUI
