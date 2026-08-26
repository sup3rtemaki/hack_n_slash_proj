#ifndef CAMERACONTROLLER
#define CAMERACONTROLLER

#include "globals.h"
#include "entity.h"

class CameraController {
public:
	Entity* target = nullptr;
	float lerp = 50.0f;
	float deltaTime = 0;
	bool isLerping;

	void update();
};

#endif // !CAMERACONTROLLER

