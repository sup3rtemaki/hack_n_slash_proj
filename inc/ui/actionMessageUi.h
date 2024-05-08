#ifndef ACTIONMESSAGEUI
#define ACTIONMESSAGEUI

#include "ui.h"

#include <string>

class ActionMessageUi : public Ui {
public:
	ActionMessageUi();

	virtual void draw() override;
	virtual void setUp() override;
	void setMessage(string message);

private:
	string message;
	string resPath;
	SDL_Texture* fontTexture;
	SDL_Texture* backgroundBarTexture;
	float timer;
	int fontX;
};

#endif // !ACTIONMESSAGEUI
