#ifndef MENU
#define MENU

#include "ui/ui.h"
class Menu : public Ui {
public:
	Menu(class Hero* hero);

	virtual void draw() override;
	virtual void setUp() override;
};

#endif // !MENU
