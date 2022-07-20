#pragma once

#include "../model.h"

class TiledModel : public Model {
public:
    TiledModel(int tilesetId, const pybind11::list& l);
private:
    GLuint m_texId;
};
