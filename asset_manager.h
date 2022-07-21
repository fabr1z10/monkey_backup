#pragma once

#include <unordered_map>
#include "models/sprite.h"
#include "font.h"
#include "tex.h"
#include "models/tiled.h"


class AssetManager {
public:
	static AssetManager& instance() {
		static AssetManager instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	std::shared_ptr<Sprite> getSprite(const std::string&);
    std::shared_ptr<TiledModel> getTiled(const std::string&);
	std::shared_ptr<Tex> getTex(const std::string&);
	std::shared_ptr<Font> getFont(const std::string&);

private:
    std::unordered_map<std::string, std::shared_ptr<TiledModel>> m_tiled;
	std::unordered_map<std::string, std::shared_ptr<Sprite>> m_sprites;
	std::unordered_map<std::string, std::shared_ptr<Tex>> m_tex;
	std::unordered_map<std::string, std::shared_ptr<Font>> m_fonts;

};