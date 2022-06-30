#include "sprite.h"
#include "asset_manager.h"
#include <iostream>
#include <yaml-cpp/yaml.h>

Sprite::Sprite(const std::string& id) {
	// strip spritename
	auto u = id.find_last_of('/');
	auto spriteName = id.substr(u + 1);
	std::cout << "spritename= " << spriteName << "\n";
	std::string file = "assets/" + id.substr(0, u) + ".yaml";
	std::cout << "file=" << file << "\n";
	auto f = YAML::LoadFile(file);
	auto& am = AssetManager::instance();

	for(YAML::const_iterator it=f.begin();it!=f.end();++it) {
		std::cout << " --- reading sprite: " << it->first.as<std::string>() << "\n";
		auto node = it->second;
		auto sheet = node["sheet"].as<std::string>();
		auto tex = am.getTex(sheet);
		std::cout << " tex loaded: " << tex->getWidth() << ", " << tex->getHeight() << "\n";
		std::cout << " --- sheet: " << sheet << std::endl;
	}
	exit(1);
}