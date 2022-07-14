#pragma once

#include <pybind11/pybind11.h>
#include <glm/glm.hpp>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

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

template<typename T>
T pyget (pybind11::object obj, const std::string& key, T defaultValue) {
	try {
		auto value = obj.attr(key.c_str()).cast<T>();
		return value;
	} catch (...) {
		return defaultValue;
	}
}

template<typename T>
T dictget (pybind11::object obj, const std::string& key, T defaultValue) {
	try {
		auto value = obj[key.c_str()].cast<T>();
		return value;
	} catch (...) {
		return defaultValue;
	}
}

template<>
inline glm::vec4 dictget(pybind11::object obj, const std::string& key, glm::vec4 defaultValue) {
    try {
        auto value = obj[key.c_str()].cast<pybind11::array_t<float>>();
        return glm::vec4(value.at(0), value.at(1), value.at(2), value.at(3));
    } catch (...) {
        return defaultValue;
    }

}



