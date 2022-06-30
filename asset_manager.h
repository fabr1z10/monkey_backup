#pragma once

#include <unordered_map>
#include "sprite.h"
#include "tex.h"


class AssetManager {
public:
	static AssetManager& instance() {
		static AssetManager instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	std::shared_ptr<Sprite> getSprite(const std::string&);
	std::shared_ptr<Tex> getTex(const std::string&);

private:
	std::unordered_map<std::string, std::shared_ptr<Sprite>> m_sprites;
	std::unordered_map<std::string, std::shared_ptr<Tex>> m_tex;

};