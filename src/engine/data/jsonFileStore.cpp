#include "jsonFileStore.h"

#include <fstream>
#include <iomanip>

namespace JsonFileStore {

JsonFileResult readJsonFile(const std::string& path, nlohmann::json& outData) {
	std::ifstream file(path);
	if (!file.is_open()) {
		return JsonFileResult::FileNotFound;
	}

	// non-throwing parse: malformed content is reported instead of crashing the caller
	outData = nlohmann::json::parse(file, nullptr, false);
	if (outData.is_discarded()) {
		return JsonFileResult::ParseError;
	}

	return JsonFileResult::Success;
}

bool writeJsonFile(const std::string& path, const nlohmann::json& data) {
	std::ofstream file(path);
	if (!file.is_open()) {
		return false;
	}

	file << std::setw(4) << data << std::endl;
	return true;
}

}
