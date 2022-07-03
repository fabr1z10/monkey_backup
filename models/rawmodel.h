#pragma once

#include "../model.h"

class RawModel : public Model {
public:
	void draw(Shader*, int, int) override;

	RawModel(int, py::array_t<float> input1, py::array_t<unsigned> elements, const py::kwargs& kwargs);
private:
	GLuint m_texId;

};