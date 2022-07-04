#include "font.h"
#include "asset_manager.h"
#include "util.h"
#include <fstream>
#include <iostream>
#include "util.h"

Font::Font(const std::string& fontId) {
	std::string filepath = "assets/" + fontId + ".desc";

	std::ifstream infile(filepath);
	std::string line;

	std::string textureFilepath;
	std::string referenceSize;

	std::getline(infile, textureFilepath);
	std::cout << " the tex filepath is " << textureFilepath << "\n";
	auto& am = AssetManager::instance();
	auto tex = am.getTex(textureFilepath);
	float tw = tex->getWidth();
	float th = tex->getHeight();
	m_texId = tex->getTexId();


	std::getline(infile, referenceSize);
	auto refSize = strToVec<float>(referenceSize);

	while (std::getline(infile, line))
	{
		auto colon = line.find(':');
		std::string chars = line.substr(0, colon);
		auto data = strToVec<int>(line.substr(colon + 1));
		auto sss = getString32(chars);
		int u = 0;
		for (char32_t w : sss) {
			if (u % 2 == 0) {
				m_info[w] = CharInfo{data[0] / tw, data[1] / th, data[2] / tw, data[3] / th, static_cast<float>(data[4]),
						 data[2] / refSize[0], data[3] / refSize[1]};
			}
			u++;
		}
	}
}

const CharInfo & Font::getCharInfo(char32_t c) {
	return m_info.at(c);
}

bool Font::hasCharInfo(char32_t c) {
	return m_info.count(c) > 0;
}