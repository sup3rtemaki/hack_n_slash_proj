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
	void setTimer(float time);
	bool isMessageEmpty();
	void lock();
	void unlock();
	bool isUiLocked();

private:
	string message;
	string prevMessage;
	string resPath;
	SDL_Texture* fontTexture;
	SDL_Texture* backgroundBarTexture;
	float timer;
	int fontX;
	bool uiLock;
};

#endif // !ACTIONMESSAGEUI
