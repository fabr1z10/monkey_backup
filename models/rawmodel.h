#pragma once

#include "../model.h"

class RawModel : public Model {
public:
//	void draw(Shader*, const glm::mat4&) override;
	RawModel(int, GLuint, const std::vector<float>& input1, const std::vector<unsigned>& elements);
	RawModel(int, const py::array_t<float>& input1, const py::array_t<unsigned>& elements, const py::kwargs& kwargs);
private:
	GLuint m_texId;

};