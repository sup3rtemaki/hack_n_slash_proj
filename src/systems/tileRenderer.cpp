#include "systems/tileRenderer.h"

#include "drawing_functions.h"
#include "renderContext.h"
#include "tileson/tileson.hpp"

#include <any>
#include <iostream>
#include <utility>

TileRenderer::TileRenderer(RenderContext& renderContext, std::string texturesPath)
	: renderContext(renderContext), texturesPath(std::move(texturesPath)) {
}

TileRenderer::~TileRenderer() {
}

void TileRenderer::TextureDeleter::operator()(SDL_Texture* texture) const {
	if (texture) {
		SDL_DestroyTexture(texture);
	}
}

SDL_Texture* TileRenderer::getTilesetTexture(
	const std::string& tilesetName,
	const std::string& tilesetFile) {
	auto search = texturesCache.find(tilesetName);
	if (search != texturesCache.end()) {
		return search->second.get();
	}

	SDL_Texture* loadedTexture = loadTexture(
		texturesPath + tilesetFile,
		renderContext.renderer);
	TexturePtr texture(loadedTexture, TextureDeleter{});
	SDL_Texture* texturePointer = texture.get();
	texturesCache.emplace(tilesetName, std::move(texture));
	return texturePointer;
}

void TileRenderer::render(tson::Map* map) {
	if (map == nullptr) {
		std::cout << "Mapa nulo" << std::endl;
		return;
	}

	int x = 0;
	int y = 0;

	for (auto layer : map->getLayers()) {
		for (auto& [pos, tileObject] : layer.getTileObjects()) {
			if (tileObject.getTile() == nullptr) {
				std::cout << "tile null" << std::endl;
				return;
			}

			if (layer.getType() != tson::LayerType::TileLayer) {
				continue;
			}

			tson::Tileset* tileset = tileObject.getTile()->getTileset();
			bool isTransparentTile = std::any_cast<bool>(
				tileObject.getTile()->getProp("isTransparent")->getValue());

			if (!isTransparentTile) {
				std::string tilesetName = tileset->getImage().filename().string();
				SDL_Texture* texture = getTilesetTexture(tilesetName, tilesetName);
				if (texture == nullptr) {
					std::cout << "texture null " << tilesetName << std::endl;
					return;
				}

				SDL_Rect tileRect;
				tileRect.x = tileObject.getDrawingRect().x;
				tileRect.y = tileObject.getDrawingRect().y;
				tileRect.w = tileObject.getDrawingRect().width;
				tileRect.h = tileObject.getDrawingRect().height;

				SDL_Rect renderTile;
				renderTile.x = (x * tileRect.w) - renderContext.camera.x;
				renderTile.y = (y * tileRect.h) - renderContext.camera.y;
				renderTile.w = tileRect.w;
				renderTile.h = tileRect.h;

				SDL_RenderCopy(renderContext.renderer, texture, &tileRect, &renderTile);
			}

			y++;
			if (y >= 32) {
				y = 0;
				x++;
				if (x >= 32) {
					x = 0;
				}
			}
		}
	}
}
