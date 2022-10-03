#pragma once


#include "../model.h"

class MultiModel : public Model {
public:
    MultiModel() : Model(0) {}
//    void draw(Shader*, const glm::mat4&) override;
    void addModel(std::shared_ptr<Model>);
private:
    // underlying models
    std::vector<std::shared_ptr<Model>> m_models;
};


