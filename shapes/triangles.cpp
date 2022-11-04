#include "triangles.h"
#include "../util.h"
#include <array>
#include "../shapes/earcut.h"
#include <iostream>

Triangles::Triangles(const std::vector<glm::vec2>& boundary) {
    m_type = ShapeType::TRIANGLES;

    m_bounds.min.x = boundary[0].x;
    m_bounds.max.x = m_bounds.min.x;
    m_bounds.min.y = boundary[0].y;
    m_bounds.max.y = m_bounds.min.y;

    using Coord = float;
    using Point = std::array<Coord, 2>;
    using N = uint32_t;

    std::vector<Point> polygon;
    size_t n = 0;
    for (size_t i = 0; i < boundary.size(); i ++) {
        float x = boundary[i].x;
        float y = boundary[i].y;
        polygon.push_back({x, y});
        m_bounds.addPoint(boundary[i]);
    }

    std::vector<std::vector<Point>> p;
    p.push_back(polygon);
    auto tri = mapbox::earcut<N>(p);
    std::cout << "number of triangles: " << tri.size() / 3 << "\n";
    for (size_t i = 0; i< tri.size(); i+=3) {
        addTriangle(boundary[tri[i]], boundary[tri[i+1]], boundary[tri[i+2]]);
    }

}

void Triangles::addTriangle(glm::vec2 A, glm::vec2 B, glm::vec2 C) {
    Tri t;
    t.axes[0] = glm::normalize(rot90(B - A));
    t.axes[1] = glm::normalize(rot90(C - B));
    t.axes[2] = glm::normalize(rot90(A - C));
    t.points = {A, B, C};
    m_triangles.push_back(t);

}

//void Triangles::addBoundaryPoint(glm::vec2 A) {
//    m_points.push_back(A);
//}

glm::vec2 Triangles::project(glm::vec2 axis, const glm::mat4 & t) const {
//    glm::vec2 out (std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
//    for (const auto& p : m_points) {
//        glm::vec2 pw = t * glm::vec4(p, 0.0f, 1.0f);
//        float pr = glm::dot (pw, axis);
//        out.x = std::min (out.x, pr);
//        out.y = std::max (out.y, pr);
//    }
//    return out;
}