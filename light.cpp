#include "light.h"

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec4 ambient, glm::vec4 diffuse) : Light(),
    m_direction(glm::normalize(direction)), m_ambientColor(ambient), m_diffuseColor(diffuse) {}


void DirectionalLight::setup(Shader* s) {
    s->setVec3("lightDir", m_direction);
    s->setVec4("ambient", m_ambientColor);
    s->setVec4("diffuse", m_diffuseColor);


}