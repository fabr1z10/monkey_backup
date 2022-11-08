#include "image.h"
#include "../asset_manager.h"
#include "../pyhelper.h"

Image::Image(const std::string& filename, const pybind11::kwargs& args) : Model(ShaderType::SHADER_TEXTURE) {

    auto size = dictget<glm::vec2>(args, "size", glm::vec2(0.f));
    auto a = dictget<glm::vec2>(args, "anchor", glm::vec2(0.f));

    auto& am = AssetManager::instance();
    auto tex = am.getTex(filename);

    if (size == glm::vec2(0.f)) {
        size.x = tex->getWidth();
        size.y = tex->getHeight();
    } else {
        a.x = (a.x / tex->getWidth()) * size.x;
        a.y = (a.y / tex->getHeight()) * size.y;

    }
    //m_depth = dictget<GLuint>(args, "depth", GL_LESS);

    m_texId = tex->getTexId();

    auto vertices = std::vector<float>({
        -a.x, -a.y, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 1.f,
        -a.x + size.x, -a.y, 0.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f,
        -a.x + size.x, -a.y + size.y, 0.f, 1.f, 0.f, 1.f, 1.f, 1.f, 1.f,
        -a.x, -a.y + size.y, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f
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




void Image::draw(Shader* s, int o, int c) {


    Model::draw(s, o, c);
}