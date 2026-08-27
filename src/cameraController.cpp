#include "cameraController.h"

void CameraController::update(SDL_Rect& camera, int worldWidth, int worldHeight) {
	if (target != NULL) {
		//we are following someone
		int targetX = (int)target->x - camera.w / 2;
		int targetY = (int)target->y - camera.h / 2;

		//move to target position with delay
		if (isLerping) {
			//condition to move: camera not great than world boundaries
			if ((target->x <= (worldWidth - (camera.w / 2))) &&
				(target->x >= (camera.w / 2))) {
				camera.x += floor(((targetX)-camera.x) * lerp * deltaTime);
			}
			else if (target->x > (worldWidth - (camera.w / 2))) {
					camera.x = floor(worldWidth - camera.w);
			}
			else if (target->x < (camera.w / 2)) {
					camera.x = 0;
			}

			if ((target->y <= (worldHeight - (camera.h / 2))) &&
				(target->y >= (camera.h / 2))) {
				camera.y += floor(((targetY)-camera.y) * lerp * deltaTime);
			}
			else if (target->y > (worldHeight - (camera.h / 2))) {
				camera.y = floor(worldHeight - camera.h);
			}
			else if (target->y < (camera.h / 2)) {
				camera.y = 0;
			}
		}
		else {
			//or fixed on target position
			camera.x = floor(targetX);
			camera.y = floor(targetY);
		}
	}
	else {
		//dont follow
	}
}