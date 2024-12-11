#ifndef MENU
#define MENU

#include "ui/ui.h"

#include <vector>

enum class MenuState {
	Active,
	Background,
	Inactive
};

class Menu : public Ui {
public:
	int index;
	MenuState menuState;

protected:
	class Hero* hero = nullptr;
	SDL_Texture* bgImage;
	SDL_Texture* fontTexture;
	vector<string> menuItems;
	int textYOffset;

	virtual void drawMenuBackground() = 0;
	virtual void drawText() = 0;
	virtual void drawSelectionBox() = 0;
	virtual void onIndexUp() = 0;
	virtual void onIndexDown() = 0;
	virtual void onIndexLeft() = 0;
	virtual void onIndexRight() = 0;
};

#endif // !MENU
