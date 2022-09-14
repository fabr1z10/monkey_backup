#include <iostream>
#include "spritesheet.h"
#include "../asset_manager.h"

SpriteSheet::SpriteSheet(const YAML::Node& node) : Model(ShaderType::SHADER_TEXTURE) {

	std::vector<GLfloat> vertices;
	std::vector<unsigned> indices;

	auto sheet = node["sheet"];
	assert(sheet != nullptr);

	auto imageFile = sheet["file"].as<std::string>();
	float ppu = sheet["ppu"].as<float>(1.f);
	auto &am = AssetManager::instance();
	auto tex = am.getTex(imageFile);
	m_texId = tex->getTexId();
	auto texw = tex->getWidth();
	auto texh = tex->getHeight();
	unsigned ix = 0;
	for (YAML::const_iterator item = sheet["items"].begin(); item != sheet["items"].end(); ++item) {
		auto key = item->first.as<std::string>();
		m_keyHelp[key] = m_itemInfo.size();
		auto appo = item->second.as<std::vector<int>>();
		auto l = appo.size();
		float ox = l > 4 ? appo[4] : 0.f;
		float oy = l > 5 ? appo[5] : 0.f;
		auto width_pixel = static_cast<float>(appo[2]);
		auto height_pixel = static_cast<float>(appo[3]);
		float tx = static_cast<float>(appo[0]) / texw;
		float ty = static_cast<float>(appo[1]) / texh;
		float tw = width_pixel / texw;
		float th = height_pixel / texh;
		int width_actual = width_pixel * ppu;
		int height_actual = height_pixel * ppu;
		unsigned offset = indices.size();

		// bottom left
		vertices.insert(vertices.end(), {ox, oy, 0.0f, tx, ty + th, 1, 1, 1, 1});
		// bottom right
		vertices.insert(vertices.end(), {ox + width_actual, oy, 0.0f, tx + tw, ty + th, 1, 1, 1, 1});
		// top right
		vertices.insert(vertices.end(), {ox + width_actual, oy + height_actual, 0.0f, tx + tw, ty, 1, 1, 1, 1});
		// top left
		vertices.insert(vertices.end(), {ox, oy + height_actual, 0.0f, tx, ty, 1, 1, 1, 1});
		indices.insert(indices.end(), {ix, ix + 1, ix + 2, ix + 3, ix, ix + 2});
		ix += 4;
		m_itemInfo.emplace_back(offset, 6);
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

void SpriteSheet::draw(Shader * s, int itemId) {

	if (m_texId != GL_INVALID_VALUE) {
		s->setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texId);
	}
	const auto& i =m_itemInfo[itemId];
	Model::draw(s, i.first, i.second);
}

int SpriteSheet::getId(const std::string &key) const {
	auto it = m_keyHelp.find(key);
	if (it == m_keyHelp.end()) {
		std::cerr << " don't have item: " << key;
		exit(1);
	}
	return it->second;
}