#include "asset_manager.h"
#include <iostream>

std::shared_ptr<Sprite> AssetManager::getSprite(const std::string & id) {
	auto it = m_sprites.find(id);
	if (it == m_sprites.end()) {
		std::cout << " --- not cached. Create new!\n";
		auto sprite = std::make_shared<Sprite>(id);
		m_sprites[id] = sprite;
		return sprite;
	}
	return it->second;
}


std::shared_ptr<Tex> AssetManager::getTex(const std::string & id) {
	auto it = m_tex.find(id);
	if (it == m_tex.end()) {
		std::cout << " --- not cached. Create new!\n";
		auto tex = std::make_shared<Tex>(id);
		m_tex[id] = tex;
		return tex;
	}
	return it->second;
}

std::shared_ptr<Font> AssetManager::getFont(const std::string & id) {
	auto it = m_fonts.find(id);
	if (it == m_fonts.end()) {
		std::cout << " --- not cached. Create new!\n";
		auto font = std::make_shared<Font>(id);
		m_fonts[id] = font;
		return font;
	}
	return it->second;
}
