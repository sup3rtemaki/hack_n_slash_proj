#include "helpers/saveHandler.h"

#include <fstream>
#include <iostream>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

const std::string SAVE_FILE_PATH = "SaveGame\\save.json";

SaveHandler::SaveHandler() : heroHp(0), heroX(0), heroY(0), currentMapId(0) {
}

SaveHandler::~SaveHandler() {
}

void SaveHandler::save(int heroHp, int heroX, int heroY, int currentMapId, std::vector<std::pair<int, int>> items) {
	std::cout << "Save\n";

	std::ofstream file(SAVE_FILE_PATH);
	if (!file.is_open()) {
		std::cout << "Erro ao abrir arquivo SaveGame\\save.json\n";
		return;
	}

	json save;
	save["heroHp"] = heroHp;
	save["heroX"] = heroX;
	save["heroY"] = heroY;
	save["currentMapId"] = currentMapId;

	if (!items.empty()) {
		for (auto i : items) {
			json jItem;
			jItem["id"] = i.first;
			jItem["itemQty"] = i.second;
			save["inventory"].push_back(jItem);
		}
	}

	file << std::setw(4) << save << std::endl;
	file.close();
}

bool SaveHandler::load() {
	std::ifstream file(SAVE_FILE_PATH);
	if (!file.is_open()) {
		std::cout << "Erro ao abrir arquivo SaveGame\\save.json\n";
		return false;
	}

	json data = json::parse(file);

	if (data.empty()) {
		std::cout << "Erro ao parsear arquivo SaveGame\\save.json\n";
		return false;
	}

	heroHp = data["heroHp"];
	heroX = data["heroX"];
	heroY = data["heroY"];
	currentMapId = data["currentMapId"];
	items.clear();

	json jItem = data["inventory"];

	if (!jItem.empty()) {
		for (auto& element : jItem) {
			items.push_back(std::make_pair(element["id"], element["itemQty"]));
		}
	}

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

std::vector<std::pair<int, int>> SaveHandler::getItems() {
	return items;
}
