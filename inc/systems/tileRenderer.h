#ifndef TILERENDERER_H
#define TILERENDERER_H

#include <SDL.h>
#include <map>
#include <memory>
#include <string>

struct RenderContext;

namespace tson {
class Map;
}

class TileRenderer {
public:
	TileRenderer(RenderContext& renderContext, std::string texturesPath);
	~TileRenderer();

	void render(tson::Map* map);

private:
	struct TextureDeleter {
		void operator()(SDL_Texture* texture) const;
	};

	using TexturePtr = std::unique_ptr<SDL_Texture, TextureDeleter>;

	RenderContext& renderContext;
	std::string texturesPath;
	std::map<std::string, TexturePtr> texturesCache;

	SDL_Texture* getTilesetTexture(const std::string& tilesetName,
		const std::string& tilesetFile);
};

#endif // TILERENDERER_H
