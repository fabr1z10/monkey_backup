#include "light.h"

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 color) : Light(),
    m_direction(glm::normalize(direction)), m_diffuseColor(color) {}


void DirectionalLight::setup(Shader* s) {
    s->setVec3("lightDir", m_direction);
    s->setVec3("diffuse", m_diffuseColor);


}