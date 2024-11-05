#ifndef MENU
#define MENU

#include "ui/ui.h"
class Menu : public Ui {
public:
	Menu(class Hero* hero);

	int index;

	virtual void draw() override;
	virtual void setUp() override;

private:
	Hero* hero = nullptr;

	void drawMenuBackground();
	void drawText();
	void drawSelectionBox();
};

#endif // !MENU
