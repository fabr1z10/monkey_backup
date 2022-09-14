#include "asset_manager.h"
#include <iostream>
#include <yaml-cpp/yaml.h>

std::shared_ptr<TiledModel> AssetManager::getTiled(const std::string & id) {
    auto it = m_tiled.find(id);
    if (it == m_tiled.end()) {
        auto u = id.find_last_of('/');
        auto tiledName = id.substr(u + 1);
        std::string file = "assets/" + id.substr(0, u) + ".yaml";
        auto f = YAML::LoadFile(file);
        for(YAML::const_iterator it=f.begin();it!=f.end();++it) {
            auto currId = it->first.as<std::string>();
            std::string cspr = id.substr(0, u+1) + currId;
            if (currId == tiledName) {
                m_tiled[cspr] = std::make_shared<TiledModel>(it->second.as<std::string>());
            }
        }
        return m_tiled.at(id);
    } else {
        return it->second;
    }

}

std::shared_ptr<Sprite> AssetManager::getSprite(const std::string & id) {
	auto it = m_sprites.find(id);
	if (it == m_sprites.end()) {
		std::cout << " --- not cached. Create new!\n";
        auto u = id.find_last_of('/');
        auto spriteName = id.substr(u + 1);
        auto sheetName = id.substr(0, u);
        std::string file = "assets/" + sheetName + ".yaml";
        auto f = YAML::LoadFile(file);
        // check if spritesheet has been loaded
        if (m_spriteSheets.count(sheetName) == 0) {
        	// load spritesheet
        	m_spriteSheets.insert(std::make_pair(sheetName, std::make_shared<SpriteSheet>(f)));
        }
		auto sheet = m_spriteSheets.at(sheetName);
		auto gino = f["sprites"];

        for(YAML::const_iterator it=gino.begin();it!=gino.end();++it) {
            auto currId = it->first.as<std::string>();
            std::string cspr = id.substr(0, u+1) + currId;
            std::cout << " --- adding sprite: " << cspr << "\n";
            m_sprites[cspr] = std::make_shared<Sprite>(it->second, sheet.get());
        }
        if (m_sprites.count(id) == 0) {
        	std::cerr << " looks like sprite: " << id << " does not exist!" << std::endl;
        	exit(1);
        }
        return m_sprites.at(id);
	} else {
        return it->second;
    }
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
