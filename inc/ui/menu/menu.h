#ifndef MENU
#define MENU

#include "ui/ui.h"

#include <vector>

class Menu : public Ui {
public:
	Menu(class Hero* hero);

	int index;

	virtual void draw() override;
	virtual void setUp() override;

private:
	Hero* hero = nullptr;
	SDL_Texture* bgImage;
	SDL_Texture* fontTexture;
	vector<string> menuItems;
	int textYOffset;

	void drawMenuBackground();
	void drawText();
	void drawSelectionBox();
};

#endif // !MENU
