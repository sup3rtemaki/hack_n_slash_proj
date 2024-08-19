#ifndef QUICKITEMUI
#define QUICKITEMUI

#include "ui.h"

#include "hero.h"

#include <string>

class QuickItemUi : public Ui {
public:
	QuickItemUi(Hero* hero);
	~QuickItemUi();

	virtual void draw() override;
	virtual void setUp() override;

private:
	SDL_Texture* quickItemFrame;
	Hero* hero = nullptr;

	void drawItemFrame();
	void drawCurrentItem();
	void drawItemQuantity();
};

#endif // !QUICKITEMUI

