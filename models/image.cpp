#include "image.h"
#include "../asset_manager.h"
#include "../pyhelper.h"

Image::Image(const std::string& filename, const pybind11::kwargs& args) : Model(ShaderType::SHADER_TEXTURE) {

    glm::vec2 size = dictget<glm::vec2>(args, "size", glm::vec2(0.f));

    auto& am = AssetManager::instance();
    auto tex = am.getTex(filename);

    if (size == glm::vec2(0.f)) {
        size.x = tex->getWidth();
        size.y = tex->getHeight();
    }

    m_texId = tex->getTexId();

    auto vertices = std::vector<float>({
        0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 1.f,
        size.x, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f,
        size.x, size.y, 0.f, 1.f, 0.f, 1.f, 1.f, 1.f, 1.f,
        0.f, size.y, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f
    });
    auto elements = std::vector<unsigned>({0, 1, 2, 2, 3, 0});

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * elements.size(), &elements[0], GL_STATIC_DRAW);

    m_elementSize = elements.size();
    m_size = m_elementSize;
}


void Image::draw(Shader* s, int offset, int size) {

    s->setInt("texture_diffuse1", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texId);

    Model::draw(s, offset, size);
}