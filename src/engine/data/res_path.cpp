#include "res_path.h"

/*
* Get the resource path for resources located in res/subDir
* It's assumed the project directory is structured like:
* bin/
*  the executable
* res/
*  Lesson1/
*  Lesson2/
*
* Paths returned will be Lessons/res/subDir
*/
std::string getResourcePath(const std::string &subDir){
	//We need to choose the path separator properly based on which
	//platform we're running on, since Windows uses a different
	//separator than most systems
#ifdef _WIN32
	const char PATH_SEP = '\\';
#else
	const char PATH_SEP = '/';
#endif

	// Resolve the executable base path on each call instead of caching it in a
	// static variable. This keeps the resource path function stateless and avoids
	// hidden process-global lifetime while preserving the same behavior.
	char *basePath = SDL_GetBasePath();
	if (!basePath) {
		std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
		return "";
	}

	std::string baseRes = basePath;
	SDL_free(basePath);

	const size_t pos = baseRes.rfind("bin");
	if (pos != std::string::npos) {
		baseRes = baseRes.substr(0, pos) + "res" + PATH_SEP;
	}
	else {
		baseRes = baseRes + "res" + PATH_SEP;
	}

	//If we want a specific subdirectory path in the resource directory
	//append it to the base path. This would be something like Lessons/res/Lesson0
	return subDir.empty() ? baseRes : baseRes + subDir + PATH_SEP;
}
