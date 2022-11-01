#include "shaderinit.h"
#include "room.h"
#include "engine.h"

void LightInitializer::init(Shader* s) {
    Engine::instance().getRoom()->useLights(s);
}

void LightPreDraw::init(Shader * s , Node * node) {
    const auto& m = node->getWorldMatrix();
    auto normalMat = glm::mat3(glm::transpose(glm::inverse(m)));
    s->setMat3("nMat", normalMat);
}