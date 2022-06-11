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
			if ((target->x <= (1024 - (Globals::camera.w / 2))) &&
				(target->x >= (Globals::camera.w / 2))) {
				Globals::camera.x += ((targetX)-Globals::camera.x) * lerp * TimeController::timeController.dT;
			}
			else if (target->x > (1024 - (Globals::camera.w / 2))) {
					Globals::camera.x = 1024 - Globals::camera.w;
			}
			else if (target->x < (Globals::camera.w / 2)) {
					Globals::camera.x = 0;
			} 

			if ((target->y <= (1024 - (Globals::camera.h / 2))) &&
				(target->y >= (Globals::camera.h / 2))) {
				Globals::camera.y += ((targetY)-Globals::camera.y) * lerp *TimeController::timeController.dT;
			}
			else if (target->y > (1024 - (Globals::camera.h / 2))) {
				Globals::camera.y = 1024 - Globals::camera.h;
			}
			else if (target->y < (Globals::camera.h / 2)) {
				Globals::camera.y = 0;
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