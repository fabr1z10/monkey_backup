#pragma once

#include <pybind11/pybind11.h>
#include <glm/glm.hpp>

class WalkArea {
public:
    WalkArea(const pybind11::kwargs& );

private:
    std::vector<glm::vec2> m_outline;
    std::vector<std::vector<glm::vec2>> m_holes;
};
