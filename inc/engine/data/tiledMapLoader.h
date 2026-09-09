#ifndef TILEDMAPLOADER
#define TILEDMAPLOADER

#include <memory>
#include <string>

namespace tson {
class Map;
}

class TiledMapLoader {
public:
	std::unique_ptr<tson::Map> load(const std::string& mapFile) const;
};

#endif