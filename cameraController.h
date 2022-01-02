#ifndef CAMERACONTROLLER
#define CAMERACONTROLLER

#include "globals.h"
#include "entity.h"

class CameraController {
public:
	Entity* target = NULL;
	float lerp = 5.0f;
	bool isLerping;

	void update();
};

#endif // !CAMERACONTROLLER

