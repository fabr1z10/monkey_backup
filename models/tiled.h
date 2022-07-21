#pragma once

#include "../model.h"

class TiledModel : public Model {
public:
    TiledModel(const std::string&);
private:
    GLuint m_texId;
};
