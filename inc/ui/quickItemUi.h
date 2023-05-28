#ifndef QUICKITEMUI
#define QUICKITEMUI

#include "ui.h"

#include "hero.h"

#include <string>

class QuickItemUi : public Ui {
public:
	QuickItemUi(Hero* hero);
	~QuickItemUi();

	void drawItemFrame();
	void drawCurrentItem();
	void drawItemQuantity();

	virtual void draw() override;
	virtual void setUp() override;

private:
	string resPath;
	SDL_Texture* quickItemFrame;
	Hero* hero = nullptr;
};

#endif // !QUICKITEMUI

