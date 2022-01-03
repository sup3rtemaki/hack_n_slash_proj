#include "cameraController.h"
#include "game.h"

void CameraController::update() {
	if (target != NULL) {
		//we are following someone
		float targetX = target->x - Globals::camera.w / 2;
		float targetY = target->y - Globals::camera.h / 2;

		//move to target position with delay
		if (isLerping) {
			//condition to move: camera not great than map boundaries
			if ((target->x < (1024 - (Globals::camera.w / 3))) &&
				(target->x > (Globals::camera.w / 3))) {
				Globals::camera.x += ((targetX)-Globals::camera.x) * lerp * TimeController::timeController.dT;
			}

			if ((target->y < (1024 - (Globals::camera.h / 3))) &&
				(target->y > (Globals::camera.h / 3))) {
				Globals::camera.y += ((targetY)-Globals::camera.y) * lerp * TimeController::timeController.dT;
			}
		}
		else {
			//or fixed on target position
				Globals::camera.x = targetX;
				Globals::camera.y = targetY;
		}
	}
	else {
		//dont follow
	}
}