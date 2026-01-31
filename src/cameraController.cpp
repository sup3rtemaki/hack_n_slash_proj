#include "cameraController.h"
#include "game.h"

void CameraController::update() {
	if (target != NULL) {
		//we are following someone
		int targetX = (int)target->x - Globals::camera.w / 2;
		int targetY = (int)target->y - Globals::camera.h / 2;

		//move to target position with delay
		if (isLerping) {
			//condition to move: camera not great than map boundaries
			if ((target->x <= (1024 - (Globals::camera.w / 2))) &&
				(target->x >= (Globals::camera.w / 2))) {
				Globals::camera.x += floor(((targetX)-Globals::camera.x) * lerp * TimeController::timeController.dT);
			}
			else if (target->x > (1024 - (Globals::camera.w / 2))) {
					Globals::camera.x = floor(1024 - Globals::camera.w);
			}
			else if (target->x < (Globals::camera.w / 2)) {
					Globals::camera.x = 0;
			} 

			if ((target->y <= (1024 - (Globals::camera.h / 2))) &&
				(target->y >= (Globals::camera.h / 2))) {
				Globals::camera.y += floor(((targetY)-Globals::camera.y) * lerp *TimeController::timeController.dT);
			}
			else if (target->y > (1024 - (Globals::camera.h / 2))) {
				Globals::camera.y = floor(1024 - Globals::camera.h);
			}
			else if (target->y < (Globals::camera.h / 2)) {
				Globals::camera.y = 0;
			}
		}
		else {
			//or fixed on target position
			Globals::camera.x = floor(targetX);
			Globals::camera.y = floor(targetY);
		}
	}
	else {
		//dont follow
	}
}