#ifndef PAUSE_MENU
#define PAUSE_MENU

#include "paginatedMenu.h"

class PauseMenu : public PaginatedMenu {
public:
	PauseMenu(Hero* hero);
	virtual void draw() override;
	virtual void onIndexUp() override;
	virtual void onIndexDown() override;
	virtual void onIndexLeft() override;
	virtual void onIndexRight() override;

private:
	vector<class Item*> inventory;
	vector<SDL_Texture*> itemsImages;
	Item* selectedItem;
	int infVisibleItemsLimit;
	int supVisibleItemsLimit;
	SDL_Rect* selectionRect;
	SDL_Texture* itemsBg;

	virtual void setUp() override;
	virtual void drawMenuBackground() override;
	virtual void drawText() override;
	virtual void drawSelectionBox() override;
	virtual void drawPage1() override;
	virtual void drawPage2() override;
	virtual void drawPage3() override;
	virtual void drawPage4() override;
	virtual void drawPage5() override;

	SDL_Point calculateRectSelectionBoxPosition();
};

#endif // !PAUSE_MENU
