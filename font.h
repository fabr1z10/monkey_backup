#pragma once

#include <string>
#include <unordered_map>
#include <GL/glew.h>

struct CharInfo {
	// texture top left
	float tx;
	float ty, tw, th;
	float oy;
	float sx, sy;
};

class Font {
public:
	Font(const std::string& file);
	const CharInfo& getCharInfo(char32_t);
	bool hasCharInfo(char32_t);
	GLuint getTexId() const;
private:
	std::unordered_map<char32_t, CharInfo> m_info;
	GLuint m_texId;

};

inline GLuint Font::getTexId() const {
	return m_texId;
}



