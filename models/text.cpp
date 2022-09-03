#include "../util.h"
#include <iostream>
#include "text.h"
#include "../asset_manager.h"


void Text::setText(const std::string& text) {
//    if (m_vbo != GL_INVALID_VALUE) {
//        glDeleteBuffers(1, &m_vbo);
//        glDeleteBuffers(1, &m_ebo);
//    }

    std::vector<GLfloat> vertices;
    std::vector<unsigned> indices;

    auto s32 = getString32(text);
    // initialize cursor position
    // default alignment is top left
    float x{0.0f};
    float y{0.0f};
    m_textSize.x = 0;
    m_textSize.y = m_fontSize;
    unsigned quadCount = 0;
    for (char32_t c : s32) {
        if (c == 32) {
            x += m_fontSize;
            continue;
        }
        if (c == 10) {
            x = 0.0f;
            y -= m_fontSize;
            m_textSize.y += m_fontSize;
            continue;
        }
        if (!m_font->hasCharInfo(c)) {
            std::cout << "*** font does not have character: " << c << "\n";
            continue;
        }
        const auto& info = m_font->getCharInfo(c);
        auto w = m_fontSize * info.sx;
        auto h = m_fontSize * info.sy;
        auto oy = info.oy;
        // generate vertices for current character
        // bottom left
        vertices.insert(vertices.end(), {x, y - oy, 0.0f, info.tx, info.ty + info.th, 1, 1, 1, 1});
        // bottom right
        vertices.insert(vertices.end(), {x + w, y - oy, 0.0f, info.tx + info.tw, info.ty + info.th, 1, 1, 1, 1});
        // top right
        vertices.insert(vertices.end(), {x + w, y + h - oy, 0.0f, info.tx + info.tw, info.ty, 1, 1, 1, 1});
        // top left
        vertices.insert(vertices.end(), {x, y + h - oy, 0.0f, info.tx, info.ty, 1, 1, 1, 1});
        m_textSize.x = std::max(m_textSize.x, x+w);
        // generate elements for current character
        unsigned ix = quadCount * 4;
        indices.insert(indices.end(), {ix, ix + 1, ix + 2, ix + 3, ix, ix + 2});
        quadCount++;

        // increment cursor position
        x += w;
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

Text::Text(const py::kwargs& args)  : Model(ShaderType::SHADER_TEXTURE) {
	auto fontId = args["font"].cast<std::string>();
	auto text = args["text"].cast<std::string>();
	m_fontSize = args["size"].cast<float>();
	auto& am = AssetManager::instance();
	m_font = am.getFont(fontId);
	m_texId = m_font->getTexId();
	setText(text);
}

Text::Text(const std::string& fontId, const std::string& text, int size) : Model(ShaderType::SHADER_TEXTURE), m_fontSize(size) {
    auto& am = AssetManager::instance();
    m_font = am.getFont(fontId);
    m_texId = m_font->getTexId();
    setText(text);


}

void Text::draw(Shader* s, int offset, int size) {

	if (m_texId != GL_INVALID_VALUE) {
		s->setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texId);
	}

	Model::draw(s, offset, size);



}

glm::vec2 Text::getSize() const {
    return m_textSize;
}