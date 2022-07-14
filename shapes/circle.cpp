#include "circle.h"

Circle::Circle(float radius) : Shape2D(), m_radius(radius) {
    m_bounds.min = glm::vec3(-m_radius, -m_radius, 0.f);
    m_bounds.max = glm::vec3(m_radius, m_radius, 0.f);
}

glm::vec2 Circle::project(glm::vec2 axis, const glm::mat4& t) const {
    glm::vec2 centerWorld = t * glm::vec4(m_offset, 1.0f);
    float x = glm::dot(centerWorld, axis);
    return glm::vec2(x - m_radius, x + m_radius);
}
