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

float computeOverlap(glm::vec2& p1, glm::vec2& p2) {
	if (p2.x >= p1.x && p2.x <= p1.y) {
		if (p2.y > p1.y) {
			return -(p1.y - p2.x);
		}
	}
	else if (p1.x >= p2.x && p1.x <= p2.y) {
		if (p1.y > p2.y) {
			return (p2.y - p1.x);
		}
	}
	float a = p2.y - p1.x;
	float b = p1.y - p2.x;
	if (abs(a) < abs(b))
		return a;
	return -b;
}