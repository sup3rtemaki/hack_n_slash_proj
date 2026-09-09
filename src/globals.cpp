#include "globals.h"

const float Globals::PI = 3.14159f;

string Globals::clipOffDataHeader(string data) {
	int pos = data.find(":", 0);// returns where we find the : in string
	if (pos != -1) {
		data = data.substr(pos + 1, data.length() - pos + 2);
	}
	return data;
}
