#ifndef SUB_MENU
#define SUB_MENU

#include "menu.h"

class SubMenu : public Menu {
public:
	SubMenu(Hero* hero);
	~SubMenu();

	void activateSubMenu(int x, int y);
	void useSelectedItem(int itemId);
	void addSelectedItemToQuickAccess(int itemId);
	void dropItem(int itemId);

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
	vector<SDL_Texture*> menuTextTextures;
	bool texturesCreated;

	int xPos;
	int yPos;

	void setUp();

};

#endif // !SUB_MENU
