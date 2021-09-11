#ifndef CAMERACONTROLLER
#define CAMERACONTROLLER

#include "globals.h"
#include "entity.h"

class CameraController {
public:
	Entity* target = NULL;
	float lerp = 2.6f;

	void update();
};

#endif // !CAMERACONTROLLER

