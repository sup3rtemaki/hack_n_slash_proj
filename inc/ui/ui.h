#ifndef UI
#define UI

class Ui {
public:
	Ui();
	~Ui();

	virtual void draw() = 0;
	virtual void setUp() = 0;
};

#endif // !UI
