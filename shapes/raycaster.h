#pragma once

#include <unordered_map>
#include <functional>
#include "../shape.h"
#include "glm/glm.hpp"
#include "../hashpair.h"
#include "../collision.h"


class RayCaster {
public:
    RayCaster();
    RayCastHit raycast(glm::vec3 A, glm::vec3 B, const Shape*, const glm::mat4&);
private:
    std::unordered_map<std::type_index, std::function<RayCastHit(const glm::vec3& A, const glm::vec3& B, const Shape*, const glm::mat4&)>> m_functionMap;
    RayCastHit rayCastPoly(const glm::vec3& A, const glm::vec3& B, const Shape *s, const glm::mat4 &t);
    void updateRaycastHit(RayCastHit& r, glm::vec2 ray, glm::vec2 line, float u, int si);

};