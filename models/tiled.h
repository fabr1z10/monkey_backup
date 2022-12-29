#pragma once

#include "../model.h"

class TiledModel : public Model {
public:
    TiledModel(const pybind11::kwargs&);
    //void draw(Shader*, const glm::mat4&) override;


};
