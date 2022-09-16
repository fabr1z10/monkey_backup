#pragma once

#include "../model.h"

class TiledModel : public Model {
public:
    TiledModel(const std::string&);
    void draw(Shader*, const glm::mat4&) override;
private:
    GLuint m_texId;
};
