#ifndef MAINMENUSYSTEM_H
#define MAINMENUSYSTEM_H

#include <SDL.h>
#include <functional>

class MainMenu;
class RenderContext;

/**
 * Encapsulates all main menu logic and rendering.
 * Handles:
 * - Input events in main menu (navigation, selection, quit)
 * - Main menu drawing
 * - State transitions
 */
class MainMenuSystem {
public:
	MainMenuSystem(MainMenu* mainMenu, RenderContext& renderContext);
	~MainMenuSystem();

	/**
	 * Process main menu events and render.
	 * @return true if should start game, false to stay in menu
	 */
	bool update(SDL_Event& event);

	/**
	 * Callback to be called when player selects "Start Game".
	 */
	void setStartGameCallback(std::function<void()> callback);

	/**
	 * Callback to be called when player wants to quit.
	 */
	void setQuitCallback(std::function<void()> callback);

private:
	MainMenu* mainMenu;
	RenderContext& renderContext;

	std::function<void()> startGameCallback;
	std::function<void()> quitCallback;

	void handleKeyDown(SDL_Keycode keycode);
	void render();
};

#endif // !MAINMENUSYSTEM_H
