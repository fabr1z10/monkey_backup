#include <locale>
#include <codecvt>
#include "util.h"

std::u32string getString32(const std::string& str) {
	return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(str.c_str());
}

// returns the angle (in radians) between two vectors
float angle(glm::vec3 v1, glm::vec3 v2) {
	return std::acos(glm::dot(v1, v2));
}