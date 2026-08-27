#ifndef JSONFILESTORE
#define JSONFILESTORE

#include <string>
#include "nlohmann/json.hpp"

enum class JsonFileResult {
	Success,
	FileNotFound,
	ParseError
};

namespace JsonFileStore {
	JsonFileResult readJsonFile(const std::string& path, nlohmann::json& outData);
	bool writeJsonFile(const std::string& path, const nlohmann::json& data);
}

#endif // !JSONFILESTORE
