#pragma once


#include "shader.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

class Model {
public:
	Model(int);
	virtual void draw(Shader*) {}
	virtual ~Model();
protected:
	int m_shaderType;
	GLuint m_vbo;
	GLuint m_ebo;
	GLuint m_primitive;
	GLuint m_elementSize;
};


class RawModel : public Model {
public:
	void draw(Shader*) override;

	RawModel(int, py::array_t<float> input1, py::array_t<unsigned> elements, const py::kwargs& kwargs);
private:
	GLuint m_texId;

};