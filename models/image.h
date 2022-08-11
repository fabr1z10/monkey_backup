#pragma once

#include "../model.h"

class Image : public Model {
public:
    Image(const std::string& file, const pybind11::kwargs& args);
    void draw(Shader*, int, int) override;

private:
    GLuint m_texId;
};
