#pragma once

#include "../component.h"

#include <pybind11/pybind11.h>
#include "glm/glm.hpp"

class Move : public Component {
public:
	Move(pybind11::function f);
	void update(double) override;

private:
	pybind11::function m_func;
	float m_time;

};

class MoveDynamics : public Component {
public:
    MoveDynamics(float);
    void update(double) override;
    void addElasticForce(float ox, float oy, float oz, float k);
    void setVelocity(float vx, float vy, float vz);
    void setMinY(float);
    void setMaxY(float);
private:
    glm::vec3 m_elasticCenter;
    float m_k;
    glm::vec3 m_velocity;
    float m_mass;
    float m_inverseMass;
    glm::vec3 m_force;
    glm::vec3 m_min;
    glm::vec3 m_max;
};

