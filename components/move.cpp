#include <iostream>
#include "move.h"
#include <pybind11/numpy.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "../node.h"
#include "../util.h"

Move::Move(pybind11::function f) : Component(), m_func(f), m_time(0.0f) {
	std::cout << "qui\n";

}

void Move::update(double dt) {
	m_time += dt;

	auto obj = m_func(m_time).cast<pybind11::array_t<float>>();
    glm::mat4 t = glm::translate(glm::vec3(obj.at(0), obj.at(1), obj.at(2)));
	glm::mat4 m = t * glm::rotate(obj.at(3), glm::vec3(0, 0, 1));
	//std::cout << m_time << ", " << obj.at(3)<< "\n";
	m_node->setModelMatrix(m);



}

MoveDynamics::MoveDynamics(float mass) : Component(), m_velocity(0.f), m_mass(mass), m_force(0.f), m_elasticCenter(0.f), m_constantForce(0.f), m_k(0.f) {
    m_min = glm::vec3(-std::numeric_limits<float>::infinity());
    m_max = glm::vec3(std::numeric_limits<float>::infinity());
    m_inverseMass = 1.0f / mass;
}

void MoveDynamics::addElasticForce(float ox, float oy, float oz, float k) {
    m_elasticCenter = glm::vec3(ox, oy, oz);
    m_k = k;
}

void MoveDynamics::setConstantForce(float fx, float fy, float fz) {
    m_constantForce = glm::vec3(fx, fy, fz);
}

void MoveDynamics::setVelocity(float vx, float vy, float vz) {
    m_velocity = glm::vec3(vx, vy, vz);
}
void MoveDynamics::setMaxY(float m) {
    m_max.y = m;
}
void MoveDynamics::setMinY(float m) {
    m_min.y = m;
}

void MoveDynamics::update(double dt) {
    auto currentPos = m_node->getWorldPosition();
    m_force = m_constantForce - m_k * (currentPos - m_elasticCenter);
    auto dtf = static_cast<float>(dt);
    m_velocity += m_force * m_inverseMass * dtf;
    auto delta = m_velocity * dtf;

    auto newPos = currentPos + delta;
    if (newPos.x < m_min.x)
        delta.x = m_min.x - currentPos.x;
    else if (newPos.x > m_max.x)
        delta.x = m_max.x - currentPos.x;
    if (newPos.y < m_min.y)
        delta.y = m_min.y - currentPos.y;
    else if (newPos.y > m_max.y)
        delta.y = m_max.y - currentPos.y;
    if (!isZero(fabs(delta.x*delta.x + delta.y*delta.y))) {
        m_node->move(glm::translate(delta));
        if (m_func) m_func(this, this->m_node);
    }
}