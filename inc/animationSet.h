#ifndef ANIMATIONSET
#define ANIMATIONSET

#include "animation.h"

using namespace std;

class AnimationSet {
public:
	string imageName;
	SDL_Texture* spriteSheet; // holds the spritesheet image for all animations
	SDL_Texture* whiteSpriteSheet = NULL; // show damage
	list<Animation> animations;

	AnimationSet();
	~AnimationSet(); // destructor - properly cleans up dynamic memory

	Animation* getAnimation(string name);

	void loadAnimationSet(string fileName, list<DataGroupType>& groupTypes, bool setColorKey = false, int transparentPixelIndex = 0, bool createWhiteTexture = false);
};

#endif // !ANIMATIONSET

