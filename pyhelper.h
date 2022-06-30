#pragma once

#include <pybind11/pybind11.h>
#include <glm/glm.hpp>
#include <pybind11/stl.h>


namespace py = pybind11;

template<typename T>
T as(pybind11::object obj) {
	throw;
}


template<>
inline glm::vec2 as(pybind11::object obj) {
	auto vec = obj.cast<std::vector<float>>();
	return glm::vec2(vec[0], vec[1]);
}

template<>
inline glm::ivec2 as(pybind11::object obj) {
	auto vec = obj.cast<std::vector<int>>();
	return glm::ivec2(vec[0], vec[1]);
}





