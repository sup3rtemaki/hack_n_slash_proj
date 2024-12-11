#ifndef PAUSE_MENU
#define PAUSE_MENU

#include "paginatedMenu.h"

class PauseMenu : public PaginatedMenu {
public:
	PauseMenu(Hero* hero);
	virtual void draw() override;

private:
	vector<class Item*> inventory;
	Item* selectedItem;
	int maxTextLines;

	virtual void setUp() override;
	virtual void drawMenuBackground() override;
	virtual void drawText() override;
	virtual void drawSelectionBox() override;
	virtual void drawPage1() override;
	virtual void drawPage2() override;
	virtual void drawPage3() override;
	virtual void drawPage4() override;
	virtual void drawPage5() override;
	virtual void onIndexUp() override;
	virtual void onIndexDown() override;
	virtual void onIndexLeft() override;
	virtual void onIndexRight() override;
};

#endif // !PAUSE_MENU
