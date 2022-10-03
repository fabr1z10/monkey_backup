#include "multi.h"


//void MultiModel::draw(Shader * s, const glm::mat4& modelMatrix) {
//    for (const auto& m : m_models) {
//        m->draw(s, modelMatrix);
//    }
//}

void MultiModel::addModel(std::shared_ptr<Model> model) {
    m_models.push_back(model);
}