#ifndef MAIN_MENU
#define MAIN_MENU

#include "menu.h"

class MainMenu : public Menu {
public:
	MainMenu();

	virtual void draw() override;
	virtual void setUp() override;
	virtual void drawMenuBackground() override;
	virtual void drawText() override;
	virtual void drawSelectionBox() override;
	virtual void onIndexUp() override;
	virtual void onIndexDown() override;
	virtual void onIndexLeft() override;
	virtual void onIndexRight() override;
};

#endif // !MAIN_MENU
