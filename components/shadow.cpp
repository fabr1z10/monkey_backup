#include "shadow.h"
#include "../node.h"
#include "../pyhelper.h"
#include "glm/gtx/transform.hpp"


Shadow::Shadow(const pybind11::kwargs& args) {
//    m_angle = t.get<float>("angle", 0.0f);
    m_length = dictget(args, "length", 1.f);
//    m_translate = t.get<glm::vec3>("translate", glm::vec3(0.0f));
    m_shear = dictget(args, "shear", 0.0f);
    m_multColor = args["color"].cast<glm::vec4>() / 255.f;
}

std::type_index Shadow::getType() {
    return std::type_index(typeid(Renderer));
}

void Shadow::start() {
    m_parent = m_node->getParent();
    m_parentRenderer = m_parent->getComponent<Renderer>();
    m_controller = dynamic_cast<Controller2D*>(m_parent->getComponent<Controller>());
    assert(m_controller != nullptr);
    //m_entity->SetPosition(glm::vec3(1/m_parent->GetScale(),0,0));
    m_multColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
}

int Shadow::setup(Shader * s) {
    if (m_parentRenderer->getModel() == nullptr || s->getShaderType() != m_parentRenderer->getModel()->getShaderType()) {
        return 1;
    }
    const auto& m = m_node->getWorldMatrix() * m_rendererTransform;
    s->setVec4("mult_color", m_multColor);
    s->setVec4("add_color", m_addColor);
    s->setMat4("model", m);
    return 0;
}


void Shadow::draw(Shader * shader) {
    m_parentRenderer->draw(shader);
}

void Shadow::update(double) {
    auto currentPos = m_parent->getWorldPosition();
    if (m_controller->grounded()) {
        m_ground = currentPos.y;
    }

    auto jump_height = currentPos.y - m_ground;
//    auto pos = m_parent->getWorldPosition();
//    bool flipx = m_parent->GetFlipX();
//    float scale = m_parent->GetScale();
//    SetTransform(m_parentRenderer->GetTransform());
//    //m_entity->SetLocalTransform(glm::translate(glm::vec3(20.0f, 0.0f, 0.0f)));
//    //m_entity->SetLocalTransform(glm::rotate(glm::radians<float>(-89.9f), glm::vec3(1.0f, 0.0f, 0.0f)));
    //auto m =glm::shearX3D(glm::mat4(1.0f), m_shear, 0.0f);
//    //m_entity->SetLocalTransform(m);
//    auto m = glm::mat4(1.0f);
//    m[1][0] = (m_parent->GetFlipX() ? -1 : 1) * m_shear;
//    m[1][1] = m_scaleLength;
//    m[3][2] = -1.0f;
    auto rot = glm::rotate(glm::radians<float>(-89.9f), glm::vec3(1.f, 0.f, 0.f));
    rot[1][0] = (m_parent->getFilpX() ? -1 : 1) * m_shear;
    rot[1][2] *= m_length;
    rot[3][1] = -jump_height / m_parent->getScale();
    rot[3][2] = -0.2f*jump_height / m_parent->getScale();
    m_node->setModelMatrix(rot);
    //node->setPosition(10.f, 0.f, 0.f);
}