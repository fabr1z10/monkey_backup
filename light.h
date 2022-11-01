#pragma once

#include <glm/glm.hpp>
#include "shader.h"

class Light {
public:

    virtual void setup(Shader*) = 0;
};


class DirectionalLight : public Light {
public:
    DirectionalLight(glm::vec3 direction, glm::vec4 ambient, glm::vec4 diffuse);
    void setup(Shader*) override;
private:
    glm::vec3 m_direction;
    glm::vec4 m_ambientColor;
    glm::vec4 m_diffuseColor;
};

