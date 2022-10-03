#include <iostream>
#include "move.h"
#include <pybind11/numpy.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "../node.h"
#include "../util.h"
#include "../pyhelper.h"

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

MoveQuat::MoveQuat(const pybind11::kwargs& args) : m_t(0.f), m_i(0), m_loop(true), m_z(0.f) {
    auto l = args["key_frames"].cast<pybind11::list>();
    m_loop = dictget(args, "loop", true);
    m_z = dictget(args, "z", 0.f);
    for (const auto& keyFrame : l) {
        auto t = keyFrame["t"].cast<float>();
        auto axis = glm::vec3(0, 0, 1); // keyFrame["axis"].cast<glm::vec3>();
        auto angle = glm::radians(keyFrame["angle"].cast<float>());
        auto pos = keyFrame["pos"].cast<glm::vec2>();
        auto dir = keyFrame["dir"].cast<glm::vec2>();
        m_keyFrames.push_back(KeyFrame{t, glm::angleAxis(angle, axis), pos, dir});
    }
    // compute control points
    for (size_t i = 0; i < m_keyFrames.size() - 1; ++i) {
        float t{0.f};
        if (seg2seg(m_keyFrames[i].pos, m_keyFrames[i].pos + m_keyFrames[i].dir,
                m_keyFrames[i+1].pos, m_keyFrames[i+1].pos + m_keyFrames[i+1].dir, t)) {
            m_keyFrames[i].controlPoint = m_keyFrames[i].pos + t * m_keyFrames[i].dir;
            m_keyFrames[i].bezierType = 2;
        } else {
            m_keyFrames[i].bezierType = 1;
        }
    }

}

void MoveQuat::update(double dt) {
    int next = m_i + 1;
    const auto& frame0 = m_keyFrames[m_i];
    const auto& frame1 = m_keyFrames[next];
    float length = frame1.time - frame0.time;
    float t = (m_t - frame0.time) / length;
    float t2 = t * t;

    glm::vec2 pos = frame0.bezierType == 2 ? (1.f - t2) * frame0.pos + 2.f * t * (1.f - t) * frame0.controlPoint + t2 * frame1.pos :
            frame0.pos + t * (frame1.pos -frame0.pos);
    // This is called a SLERP: Spherical Linear intERPolation. With GLM, you can do this with mix:
    glm::quat interpolatedquat = glm::mix(frame0.quat, frame1.quat, t); // or whatever factor
    glm::mat4 mat = glm::translate(glm::vec3(pos.x, pos.y, m_z)) * glm::toMat4(interpolatedquat);
    m_node->setModelMatrix(mat);
    m_t += static_cast<float>(dt);
    if (m_t >= frame1.time) {
        m_i++;
        if (m_i == m_keyFrames.size() - 1) {
            if (m_loop) {
                m_i = 0;
                m_t = 0;
            } else {
                m_i--;
                m_t = frame1.time;
            }
        }
    }

}