#include "stringUtils.h"

namespace StringUtils {

std::string clipOffDataHeader(std::string data) {
	int pos = static_cast<int>(data.find(":", 0));
	if (pos != -1) {
		data = data.substr(pos + 1, data.length() - pos + 2);
	}
	return data;
}

}
