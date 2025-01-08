#ifndef SUB_MENU
#define SUB_MENU

#include "menu.h"

class SubMenu : public Menu {
public:
	SubMenu(Hero* hero);

	void activateSubMenu(int x, int y);

	// Herdado por meio de Menu
	virtual void draw() override;
	virtual void drawMenuBackground() override;
	virtual void drawText() override;
	virtual void drawSelectionBox() override;
	virtual void onIndexUp() override;
	virtual void onIndexDown() override;
	virtual void onIndexLeft() override;
	virtual void onIndexRight() override;

private:
	int xPos;
	int yPos;
	void setUp();

};

#endif // !SUB_MENU
