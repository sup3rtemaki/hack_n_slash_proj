#ifndef UI
#define UI

#include "drawing_functions.h"
#include "res_path.h"

class Ui {
public:
	string RES_PATH;
	string FONT_FILE = "Berylium.ttf";;
	static const string HUD_TEXTURES_PATH;
	static const string FONTS_PATH;

	Ui();
	~Ui();

	virtual void draw() = 0;
	virtual void setUp();

protected:
	void drawHorizontalGradientBox(SDL_Renderer* renderer,
		const int x, const int y, const int w, const int h, const float steps,
		const SDL_Color c1, const SDL_Color c2, const int fill);
	void drawVerticalGradientBox(SDL_Renderer* renderer,
		const int x, const int y, const int w, const int h, const float steps,
		const SDL_Color c1, const SDL_Color c2, const int fill);
};

#endif // !UI
