#pragma once

#include "../model.h"

class Image : public Model {
public:
    Image(const std::string& file, const pybind11::kwargs& args);
    void draw(Shader*, const glm::mat4& modelMatrix) override;

private:
    GLuint m_texId;
};
