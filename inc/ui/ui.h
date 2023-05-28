#ifndef UI
#define UI

#include "drawing_functions.h"
#include "res_path.h"

class Ui {
public:
	Ui();
	~Ui();

	virtual void draw() = 0;
	virtual void setUp() = 0;
};

#endif // !UI
