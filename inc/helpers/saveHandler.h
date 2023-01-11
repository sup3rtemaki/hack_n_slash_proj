#ifndef SAVEHANDLER
#define SAVEHANDLER

#include <list>

class SaveHandler{
public:
	SaveHandler();
	~SaveHandler();

	void save(int heroHp, int heroX, int heroY, int currentMapId);
	bool load();

	int getHeroHp();
	int getHeroX();
	int getHeroY();
	int getCurrentMapId();

private:
	int heroHp;
	int heroX;
	int heroY;
	int currentMapId;
};

#endif // !SAVEHANDLER
