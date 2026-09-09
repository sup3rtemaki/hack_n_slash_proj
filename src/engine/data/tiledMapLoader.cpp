#include "tiledMapLoader.h"

#include <filesystem>
#include <iostream>

#include "tileson/tileson.hpp"

std::unique_ptr<tson::Map> TiledMapLoader::load(const std::string& mapFile) const {
	tson::Tileson tileson;
	std::unique_ptr<tson::Map> map = tileson.parse(std::filesystem::path(mapFile));

	if (map == nullptr) {
		std::cout << "Mapa nulo" << std::endl;
		return nullptr;
	}

	if (map->getStatus() != tson::ParseStatus::OK) {
		std::cout << map->getStatusMessage() << std::endl;
	}

	return map;
}