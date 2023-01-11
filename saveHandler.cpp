#include "helpers/saveHandler.h"

#include <fstream>
#include <iostream>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

SaveHandler::SaveHandler() : heroHp(0), heroX(0), heroY(0), currentMapId(0) {
}

SaveHandler::~SaveHandler() {
}

void SaveHandler::save(int heroHp, int heroX, int heroY, int currentMapId) {
	std::cout << "Save\n";

	json save;
	save["heroHp"] = heroHp;
	save["heroX"] = heroX;
	save["heroY"] = heroY;
	save["currentMapId"] = currentMapId;

	std::ofstream file("SaveGame\\save.json");
	if (!file.is_open()) {
		std::cout << "Erro ao abrir arquivo SaveGame/save.json\n";
		return;
	}
	file << std::setw(4) << save << std::endl;
	file.close();
}

bool SaveHandler::load() {
	std::ifstream file("SaveGame\\save.json");
	json data = json::parse(file);

	heroHp = data["heroHp"];
	heroX = data["heroX"];
	heroY = data["heroY"];
	currentMapId = data["currentMapId"];

	return true;
}

int SaveHandler::getHeroHp() {
	return heroHp;
}

int SaveHandler::getHeroX() {
	return heroX;
}

int SaveHandler::getHeroY() {
	return heroY;
}

int SaveHandler::getCurrentMapId() {
	return currentMapId;
}
