#include "sprite.h"
#include "../asset_manager.h"
#include <iostream>
#include <yaml-cpp/yaml.h>
#include "../symbols.h"
#include "../components/renderer.h"

Sprite::Sprite(const std::string& id) : Model(ShaderType::SHADER_TEXTURE), m_defaultAnimation(std::string()) {
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
		m_texId = tex->getTexId();
		auto texw = tex->getWidth();
		auto texh = tex->getHeight();
		std::cout << " tex loaded: " << tex->getWidth() << ", " << tex->getHeight() << "\n";
		std::cout << " --- sheet: " << sheet << std::endl;
		float ppu = 1.0f;
		int quadCount = 0;
		for (YAML::const_iterator anit = node["animations"].begin(); anit != node["animations"].end(); ++anit) {
			std::cout << " --- anim: " << anit->first << "\n";
			if (m_defaultAnimation.empty()) m_defaultAnimation = anit->first.as<std::string>();
			std::vector<FrameInfo> frameInfos;
			AnimInfo animInfo;
			animInfo.loop = anit->second["loop"].as<bool>(false);
			animInfo.frames = 0;
			for (const auto& el : anit->second["elements"]) {
				FrameInfo fi;
				fi.offset = indices.size();
				fi.count = 0;
				fi.ticks = 1;
				animInfo.frames++;
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
					fi.count += 6;
				}
				frameInfos.push_back(fi);

				//frit->second["quads"];
				//for (auto quit = frit->second["quads"].begin(); quit != frit->second["quads"].end(); ++quit) {
				//	//quit->second.as<std::vector<float>>();
				//}
			}
			m_frameInfo[anit->first.as<std::string>()] = frameInfos;
			m_animInfo[anit->first.as<std::string>()] = animInfo;
		}
		std::cout << " --- loaded " << quadCount << " quads\n";
	}

	// generate buffers
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

	m_size = indices.size();

}

void Sprite::draw(Shader* s, int offset, int size) {

	if (m_texId != GL_INVALID_VALUE) {
		s->setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texId);
	}

	Model::draw(s, offset, size);



}

const FrameInfo & Sprite::getFrameInfo(const std::string &anim, int frame) {
	return m_frameInfo[anim][frame];
}

std::shared_ptr<Renderer> Sprite::getRenderer() const {
	return std::make_shared<SpriteRenderer>(m_defaultAnimation);

}