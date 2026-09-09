#ifndef GLOBALS
#define GLOBALS

#include <string>
#include <iostream>
#include <SDL.h>
#include "randomNumber.h"

using namespace std;

class Globals {
public:
	//math
	static const float PI;

	//clips off header
	static string clipOffDataHeader(string data);
};

#endif // !GLOBALS

