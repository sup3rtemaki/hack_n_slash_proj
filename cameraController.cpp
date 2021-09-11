#include "cameraController.h"

void CameraController::update() {
	if (target != NULL) {
		//we are following someone
		float targetX = target->x - Globals::camera.w / 2;
		float targetY = target->y - Globals::camera.h / 2;

		//move to target position with delay
		Globals::camera.x += ((targetX) - Globals::camera.x) * lerp * TimeController::timeController.dT;
		Globals::camera.y += ((targetY) - Globals::camera.y) * lerp * TimeController::timeController.dT;
		
		//or fixed on target position
		//Globals::camera.x = targetX;
		//Globals::camera.y = targetY;
	}
	else {
		//dont follow
	}
}