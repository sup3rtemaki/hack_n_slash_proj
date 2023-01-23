#ifndef QUICKITEMUI
#define QUICKITEMUI

#include "ui.h"

#include "drawing_functions.h"
#include "res_path.h"
#include "hero.h"

#include <string>

class QuickItemUi : public Ui {
public:
	static const string& ITEM_FRAME_FILE;
	static const string& ITEM_QUANTITY_FONT_FILE;
	static const int ITEM_FRAME_X;
	static const int ITEM_FRAME_Y;
	static const int ITEM_X;
	static const int ITEM_Y;
	static const int ITEM_QUANTITY_FONT_X;
	static const int ITEM_QUANTITY_FONT_Y;
	static const int ITEM_QUANTITY_FONT_SIZE;
	static const SDL_Color color;

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

