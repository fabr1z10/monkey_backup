#include "sprite.h"
#include "asset_manager.h"
#include <iostream>
#include <yaml-cpp/yaml.h>
#include "symbols.h"

Sprite::Sprite(const std::string& id) : Model(ShaderType::SHADER_TEXTURE) {
	// strip spritename
	auto u = id.find_last_of('/');
	auto spriteName = id.substr(u + 1);
	std::cout << "spritename= " << spriteName << "\n";
	std::string file = "assets/" + id.substr(0, u) + ".yaml";

	std::cout << "file=" << file << "\n";
	auto f = YAML::LoadFile(file);
	auto& am = AssetManager::instance();

	std::vector<GLfloat> vertices;
	std::vector<unsigned> indices;

	for(YAML::const_iterator it=f.begin();it!=f.end();++it) {
		std::cout << " --- reading sprite: " << it->first.as<std::string>() << "\n";
		auto node = it->second;
		auto sheet = node["sheet"].as<std::string>();
		auto tex = am.getTex(sheet);
		auto texw = tex->getWidth();
		auto texh = tex->getHeight();
		std::cout << " tex loaded: " << tex->getWidth() << ", " << tex->getHeight() << "\n";
		std::cout << " --- sheet: " << sheet << std::endl;
		float ppu = 1.0f;
		int quadCount = 0;
		for (YAML::const_iterator anit = node["animations"].begin(); anit != node["animations"].end(); ++anit) {
			std::cout << " --- anim: " << anit->first << "\n";
			for (const auto& el : anit->second["elements"]) {
				for (const auto& quad : el["quads"]) {
					auto n = quad.size();
					int width_px = quad[2].as<int>();						// quad width in pixels
					int height_px = quad[3].as<int>();						// quad height in pixels
					float tx = quad[0].as<float>() / texw;
					float ty = quad[1].as<float>() / texh;
					float tw = static_cast<float>(width_px) / texw;
					float th = static_cast<float>(height_px) / texh;
					float ox = (n > 4) ? quad[4].as<float>() : 0.f;
					float oy = (n > 5) ? quad[5].as<float>() : 0.f;
					float width_actual = static_cast<float>(width_px) / ppu;
					float height_actual = static_cast<float>(height_px) / ppu;
					// TODO add flip
					// bottom left
					vertices.insert(vertices.end(), {ox, oy, 0.0f, tx, ty + th, 1, 1, 1, 1});
					// bottom right
					vertices.insert(vertices.end(), {ox + width_actual, oy, 0.0f, tx + tw, ty + th, 1, 1, 1, 1});
					// top right
					vertices.insert(vertices.end(), {ox + width_actual, oy + height_actual, 0.0f, tx + tw, ty, 1, 1, 1, 1});
					// top left
					vertices.insert(vertices.end(), {ox, oy + height_actual, 0.0f, tx, ty, 1, 1, 1, 1});
					unsigned ix = quadCount * 4;
					indices.insert(indices.end(), {ix, ix + 1, ix + 2, ix + 3, ix, ix + 2});
					quadCount++;
				}


				//frit->second["quads"];
				//for (auto quit = frit->second["quads"].begin(); quit != frit->second["quads"].end(); ++quit) {
				//	//quit->second.as<std::vector<float>>();
				//}
			}

		}
		std::cout << " --- loaded " << quadCount << " quads\n";
	}

	exit(1);
}