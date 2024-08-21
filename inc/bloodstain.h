#ifndef BLOODSTAIN
#define BLOODSTAIN

#include "entity.h"

class Bloodstain : public Entity {
public:
	static const string BLOODSTAIN_ANIM_IDLE;
	static const string BLOODSTAIN_ANIM_DIE;

	static const int BLOODSTAIN_STATE_IDLE;
	static const int BLOODSTAIN_STATE_DIE;

	bool isLive;
	string mapName;

	Bloodstain(AnimationSet* animSet);
	~Bloodstain();

	void update();
	void changeAnimation(int newState, bool resetFrameToBeginning, string animName = "");
	void updateAnimation();
	void setLocation(int posX, int posY, int essence, string mapName);
	void create();
	void destroy();
	int recoverEssence();

private:
	int prevX;
	int prevY;
};

#endif // !BLOODSTAIN
