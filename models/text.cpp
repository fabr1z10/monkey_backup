#include "../util.h"
#include <iostream>
#include "text.h"
#include "../asset_manager.h"
#include "../pyhelper.h"

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
    m_textBounds = glm::vec4(0.f);
    unsigned quadCount = 0;
    // keep track xmax for each row (xmin is always 0)
    std::vector<RowInfo> xBounds;

    for (char32_t c : s32) {
        if (c == 32) {
            x += m_fontSize;
            continue;
        }
        if (c == 10) {
            xBounds.push_back(RowInfo{x, quadCount});
            x = 0.0f;
            y -= m_fontSize;
            //m_textSize.y += m_fontSize;
            // newline
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
        //m_textSize.x = std::max(m_textSize.x, x+w);
        // generate elements for current character
        unsigned ix = quadCount * 4;
        indices.insert(indices.end(), {ix, ix + 1, ix + 2, ix + 3, ix, ix + 2});
        quadCount++;

        // increment cursor position
        x += w;
    }
    xBounds.push_back(RowInfo{x, quadCount});
    if (m_halign != HorizontalAlign::LEFT) {
        int j = 0;
        float k = (m_halign == HorizontalAlign::CENTER ? 0.5f : 1.f);
        for (int i = 0; i < xBounds.size(); ++i) {
            float dx = xBounds[i].xMax * k;
            for (; j < xBounds[i].endIndex; ++j) {
                vertices[j * 36] -= dx;
                vertices[j * 36 + 9 ] -= dx;
                vertices[j * 36 + 18] -= dx;
                vertices[j * 36+ 27] -= dx;
            }
        }
    }
    m_textBounds = glm::vec4(vertices[0], vertices[0], vertices[1], vertices[1]);
    for (size_t i = 0; i < vertices.size(); i += 9) {
        m_textBounds.x = std::min(m_textBounds.x, vertices[i]);
        m_textBounds.y = std::max(m_textBounds.y, vertices[i]);
        m_textBounds.z = std::min(m_textBounds.z, vertices[i+1]);
        m_textBounds.w = std::max(m_textBounds.w, vertices[i+1]);

    }


    for (size_t i = 0; i < xBounds.size(); ++i) {
        std::cout << "row " << i << ", " << xBounds[i].xMax << ", " << xBounds[i].endIndex << "\n";
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
	m_halign = static_cast<HorizontalAlign>(dictget<int>(args, "halign", HorizontalAlign::LEFT));
	auto& am = AssetManager::instance();
	m_font = am.getFont(fontId);
	if (m_font->hasPalette()) {
	    m_shaderType = ShaderType::SHADER_TEXTURE_PALETTE;
	}
	m_texId = m_font->getTexId();
	auto palette = dictget<std::string>(args, "palette", "");
	if (!palette.empty()) {
	    auto pal = am.getPalette(palette);
        m_paletteId = pal->getTexId();
	}

	setText(text);
}

Text::Text(const std::string& fontId, const std::string& text, int size) : Model(ShaderType::SHADER_TEXTURE), m_fontSize(size) {
    auto& am = AssetManager::instance();
    m_font = am.getFont(fontId);
    m_texId = m_font->getTexId();
    setText(text);


}

//void Text::draw(Shader* s, const glm::mat4& m) {
//
//	if (m_texId != GL_INVALID_VALUE) {
//		s->setInt("texture_diffuse1", 0);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, m_texId);
//	}
//
//	Model::draw(s, m);
//
//
//
//}

glm::vec4 Text::getSize() const {
    return m_textBounds;
}