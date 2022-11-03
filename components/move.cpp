#include <iostream>
#include "move.h"
#include <pybind11/numpy.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "../node.h"
#include "../util.h"
#include "../pyhelper.h"

MoveTranslate::MoveTranslate(const pybind11::kwargs& args) {
    auto pts = args["points"].cast<pybind11::dict>();
    m_z = dictget<float>(args, "z", 0.f);
    m_loopType = dictget<int>(args, "loop", 0);
    int current{0};
    float previousTime;
    glm::vec2 previous;
    for (const auto& p : pts) {
        auto time = p.first.cast<float>();
        auto pos = p.second.cast<glm::vec2>();
        if (current > 0) {
            Step step;
            step.startPosition = previous;
            step.endPosition = pos;
            step.endTime = time;
            step.velocity = (pos - previous) / (time - previousTime);
            m_steps.push_back(step);
        }
        current++;
        previousTime = time;
        previous = pos;
    }
    m_maxIndex = m_steps.size()-1;
}


void MoveTranslate::start() {
    m_index = 0;
    const auto& step = m_steps[0];
    m_node->setPosition(step.startPosition.x, step.startPosition.y, m_z);
    m_time = 0.f;
}

void MoveTranslate::update(double dt) {
    auto dtf = static_cast<float>(dt);
    m_time += dtf;
    const auto& currentStep = m_steps[m_index];
    glm::vec3 delta;
    if (m_time < currentStep.endTime) {
        delta = glm::vec3(currentStep.velocity * dtf, 0.f);
    } else {
        delta = glm::vec3(currentStep.endPosition, 0.f) - m_node->getLocalPosition();
        m_index++;
        if (m_index > m_maxIndex) {
           m_index = 0;
           m_time = 0;
           delta = glm::vec3(m_steps[0].startPosition, 0.f) - m_node->getLocalPosition();
        }
    }
    m_node->move(delta);

}

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

    for (const auto& keyFrame : l) {

        auto t = keyFrame["t"].cast<float>();
        if (!m_keyFrames.empty()) {
            m_keyFrames.back().endTime = t;
        }
        auto scale = dictget<float>(keyFrame.cast<pybind11::object>(), "scale", 1.f);
        auto angle = glm::radians(keyFrame["angle"].cast<float>());
        auto pos = keyFrame["pos"].cast<glm::vec3>();
        m_keyFrames.push_back(KeyFrame{t, t, pos, angle, scale});
    }

}

void MoveQuat::update(double dt) {
    const auto& currentFrame = m_keyFrames[m_i];
    const auto& nextFrame = m_keyFrames[m_i + 1];
    float k = (m_t - currentFrame.startTime) / (currentFrame.endTime - currentFrame.startTime);
    // interpolate position angle and scale
    glm::vec3 pos = currentFrame.position + k * (nextFrame.position - currentFrame.position);
    float angle = currentFrame.angle + k * (nextFrame.angle - currentFrame.angle);
    float scale = currentFrame.scale + k * (nextFrame.scale - currentFrame.scale);
    //glm::mat4 mat = glm::scale(glm::vec3(scale)) * glm::rotate(angle, glm::vec3(0.f, 0.f, 1.f)) * glm::translate(pos);
    auto mat =  glm::translate(pos) * glm::rotate(angle, glm::vec3(0.f, 0.f, 1.f))* glm::scale(glm::vec3(scale));
    m_node->setModelMatrix(mat);
    // increment time
    m_t += static_cast<float>(dt);
    if (m_t >= currentFrame.endTime) {
        // increment keyframe
        m_i ++;
        if (m_i == m_keyFrames.size() - 1) {
            m_i = 0;
            m_t = 0.f;
        }

    }


}