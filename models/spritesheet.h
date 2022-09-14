#pragma once

#include "../model.h"
#include <yaml-cpp/yaml.h>


class SpriteSheet : public Model {
public:
	explicit SpriteSheet(const YAML::Node& node);
	void draw(Shader*, int itemId);
	int getId(const std::string& key) const;
private:
	GLuint m_texId;
	std::vector<std::pair<int, int>> m_itemInfo;
	std::unordered_map<std::string, int> m_keyHelp;
};

