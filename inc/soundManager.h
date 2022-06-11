#ifndef SOUNDMANAGER
#define SOUNDMANAGER

#include "SDL_mixer.h"
#include <string>
#include <list>

using namespace std;

struct soundListing {
	Mix_Chunk* sound;
	string name;
};

class SoundManager {
public:
	list<soundListing> sounds;
	SoundManager();
	~SoundManager();
	void loadSound(string name, string file);
	void playSound(string name);

	static SoundManager soundManager;
};

#endif // !SOUNDMANAGER

