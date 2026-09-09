#ifndef PAUSEMENUSYSTEM_H
#define PAUSEMENUSYSTEM_H

#include <SDL.h>
#include <functional>

class PauseMenu;
class TimeController;
class RenderContext;

/**
 * Encapsulates all pause menu logic and rendering.
 * Handles:
 * - Input events while paused (navigation, menu selection)
 * - Pause menu drawing
 * - State transitions (Active, Inactive, Background)
 */
class PauseMenuSystem {
public:
	PauseMenuSystem(PauseMenu* pauseMenu, TimeController& gameTime, RenderContext& renderContext);
	~PauseMenuSystem();

	/**
	 * Process pause menu events and render.
	 * @return true if should resume game, false to stay paused
	 */
	bool update(SDL_Event& event);

	/**
	 * Callback to be called when game needs to resume.
	 * Example: setResumeGameCallback([]() { gameState = GameState::InGame; });
	 */
	void setResumeGameCallback(std::function<void()> callback);

	/**
	 * Callback to be called when player quits to main menu.
	 * Example: setQuitGameCallback([]() { gameState = GameState::MainMenu; });
	 */
	void setQuitGameCallback(std::function<void()> callback);

	/**
	 * Callback to check if the game should quit entirely.
	 * Example: setCheckQuitCallback([]() { return shouldQuit; });
	 */
	void setCheckQuitCallback(std::function<bool()> callback);

	/**
	 * Callback when player sets a quit flag (e.g., &quit).
	 */
	void setQuitFlagCallback(std::function<void(bool)> callback);

private:
	PauseMenu* pauseMenu;
	TimeController& gameTime;
	RenderContext& renderContext;

	std::function<void()> resumeGameCallback;
	std::function<void()> quitGameCallback;
	std::function<bool()> checkQuitCallback;
	std::function<void(bool)> quitFlagCallback;

	void handleKeyDown(SDL_Keycode keycode);
	void render();
};

#endif // !PAUSEMENUSYSTEM_H
