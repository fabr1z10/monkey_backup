#pragma once

#include <string>
#include <unordered_map>

struct CharInfo {
	float tx, ty, tw, th;
	float oy;
	float sx, sy;
};

class Font {
public:
	Font(const std::string& file);

private:
	std::unordered_map<char32_t, CharInfo> m_info;
};