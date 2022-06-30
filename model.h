#pragma once


#include "shader.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

class Model {
public:
	Model();
	virtual void draw(Shader*) {}
	virtual ~Model();
protected:
	GLuint m_vbo;
};


class RawModel : public Model {
public:
	void draw(Shader*) override;

	RawModel(py::array_t<float> input1);
};