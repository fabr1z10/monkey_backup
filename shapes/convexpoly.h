#pragma once

#include "../shape.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <vector>

namespace py = pybind11;

class ConvexPoly : public Shape2D {
public:
	ConvexPoly() = default;
	explicit ConvexPoly(const py::array_t<float>& input);

	glm::vec2 project(glm::vec2, const glm::mat4&) const override;

protected:
	std::vector<glm::vec2> m_points;
	std::vector<glm::vec2> m_edges;
};


class Rect : public ConvexPoly {
public:
	Rect(float w, float h, const py::kwargs&);

};




