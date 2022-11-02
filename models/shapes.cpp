#include "shapes.h"
#include "../pyhelper.h"
#include "../engine.h"
#include <glm/gtx/transform.hpp>
#include "../shapes/earcut.h"


void Pixels::PixelRenderer::draw(Shader * s) {
    int pixelScale = Engine::instance().getPixelScale();
    float offset = pixelScale == 1 ? 0.f : 0.5f;
    m_rendererTransform = glm::translate(glm::vec3(offset, offset, 0.f));
    Renderer::draw(s);

    //Model::draw(shader, offset, count);
}

std::shared_ptr<Renderer> Pixels::getRenderer() const {
    return std::make_shared<PixelRenderer>();

}

Pixels::Pixels(const pybind11::kwargs& args) : Model(ShaderType::SHADER_COLOR) {
    m_primitive = GL_POINTS;
    auto z = dictget<float>(args, "z", 0.f);
    auto color = dictget<glm::vec4>(args, "color", glm::vec4(255.f))/255.f;

    auto points = args["points"].cast<std::vector<unsigned>>();
    std::vector<float> vertices;
    std::vector<unsigned> elements;

    for (size_t i = 0, j=0; i < points.size(); i+=2, j++) {
        vertices.insert(vertices.end(), {static_cast<float>(points[i]), static_cast<float>(points[i+1]), z, color.r, color.g, color.b, color.a});
        elements.push_back(j);
    }
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * elements.size(), &elements[0], GL_STATIC_DRAW);

    m_elementSize = elements.size();
    m_size = m_elementSize;
}

//void Pixels::draw(Shader *shader, int offset, int count) {
//
//}

LineModel::LineModel(const pybind11::kwargs& args) : Model(ShaderType::SHADER_COLOR) {
    m_primitive = GL_LINES;
    auto a = args["a"].cast<pybind11::array_t<float>>();
    auto b = args["b"].cast<pybind11::array_t<float>>();
    auto c = dictget<glm::vec4>(args, "color", glm::vec4(255.f))/255.f;
    auto z = dictget<float>(args, "z", 0.f);



    auto vertices = std::vector<float>({
        a.at(0), a.at(1), z, c.r, c.g, c.b, c.a,
        b.at(0), b.at(1), z, c.r, c.g, c.b, c.a
    });
    auto elements = std::vector<unsigned>({0, 1});

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * elements.size(), &elements[0], GL_STATIC_DRAW);

    m_elementSize = elements.size();
    m_size = m_elementSize;
}

PolygonModel::PolygonModel(const std::vector<glm::vec2>& data) : Model(ShaderType::SHADER_COLOR) {
    using Coord = float;
    using Point = std::array<Coord, 2>;
    using N = uint32_t;

    m_primitive = GL_LINES;
    std::vector<float> vertices ;
    std::vector<unsigned> elements;
    std::vector<Point> polygon;
    size_t n = 0;
    for (size_t i = 0; i < data.size(); i ++) {
        float x = data[i].x;
        float y = data[i].y;
        polygon.push_back({x, y});
        vertices.insert(vertices.end(), {x, y, 0.0f, 1.f, 0.f, 0.f, 1.f});
        elements.push_back(n++);
    }
    elements.push_back(0);

    std::vector<std::vector<Point>> p;
    p.push_back(polygon);
    auto tri = mapbox::earcut<N>(p);
    std::cout << "number of triangles: " << tri.size() / 3 << "\n";

    this->generateBuffers(vertices, elements);

}

RectModel::RectModel(const pybind11::kwargs& args) : Model(ShaderType::SHADER_COLOR) {

    m_primitive = GL_TRIANGLES;
    auto size = args["size"].cast<pybind11::array_t<float>>();
    auto c = dictget<glm::vec4>(args, "color", glm::vec4(255.f))/255.f;
    auto z = dictget<float>(args, "z", 0.f);

    auto vertices = std::vector<float>({
        0.f, 0.f, z, c.r, c.g, c.b, c.a,
        size.at(0), 0.f, z, c.r, c.g, c.b, c.a,
        size.at(0), size.at(1), z, c.r, c.g, c.b, c.a,
        0.f, size.at(1), z, c.r, c.g, c.b, c.a });
    auto elements = std::vector<unsigned>({0, 1, 2, 0, 2, 3});

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * elements.size(), &elements[0], GL_STATIC_DRAW);

    m_elementSize = elements.size();
    m_size = m_elementSize;

}