#include "multi.h"


void MultiModel::draw(Shader * s, int offset, int size) {
    for (const auto& m : m_models) {
        m->draw(s, offset, size);
    }
}

void MultiModel::addModel(std::shared_ptr<Model> model) {
    m_models.push_back(model);
}