#include "plane.h"
#include "../pyhelper.h"
#include "../asset_manager.h"

// orientation (can be x y z)
Plane::Plane(const pybind11::kwargs& args) : Model(ShaderType::SHADER_TEXTURE_LIGHT) {
    m_primitive = GL_TRIANGLES;
    auto orientation = dictget<char>(args, "normal", 'x');
    auto width = args["width"].cast<float>();
    auto height = args["height"].cast<float>();

    auto sheetFile = args["tex"].cast<std::string>();
    auto tex_repeat = args["tex_repeat"].cast<glm::vec2>();
    float tw = width / tex_repeat.x;
    float th = height / tex_repeat.x;
    auto& am = AssetManager::instance();
    auto tex = am.getTex(sheetFile);
    m_texId = tex->getTexId();

    std::vector<float> vertices;
    if (orientation == 'x') {
        vertices = std::vector<float>({
                                              0.f, 0.f, width, 0.f, 0.f, 1.f, 0.f, 0.f,
                                              0.f, 0.f, 0.f, tw, 0.f, 1.f, 0.f, 0.f,
                                              0.f, height, 0.f, tw, th, 1.f, 0.f, 0.f,
                                              0.f, height, width, 0.f, th, 1.f, 0.f, 0.f});
    } else if (orientation == 'y') {
        vertices = std::vector<float>({
                                              0.f, 0.f, height, 0.f, 0.f, 0.f, 1.f, 0.f,
                                              width, 0.f, height, tw, 0.f, 0.f, 1.f, 0.f,
                                              width, 0.f, 0.f, tw, th, 0.f, 1.f, 0.f,
                                              0.f, 0.f, 0.f, 0.f, th, 0.f, 1.f, 0.f});
    } else if (orientation == 'z') {
        vertices = std::vector<float>({
                                              0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
                                              width, 0.f, 0.f, tw, 0.f, 0.f, 0.f, 1.f,
                                              width, height, 0.f, tw, th, 0.f, 0.f, 1.f,
                                              0.f, height, 0.f, 0.f, th, 0.f, 0.f, 1.f});
    }
    auto elements = std::vector<unsigned>({0, 1, 2, 3, 0, 2});

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * elements.size(), &elements[0], GL_STATIC_DRAW);

    m_elementSize = elements.size();
    m_size = m_elementSize;
}