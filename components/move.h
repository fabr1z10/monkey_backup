#pragma once

#include "../component.h"

#include <pybind11/pybind11.h>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

class Move : public Component {
public:
	Move(pybind11::function f);
	void update(double) override;

private:
	pybind11::function m_func;
	float m_time;

};

class MoveQuat : public Component {
public:
    explicit MoveQuat(const pybind11::kwargs&);
    void update(double) override;
private:
    struct KeyFrame {
        float time;
        glm::quat quat;
        glm::vec2 pos;
        glm::vec2 dir;
        glm::vec2 controlPoint;
        int bezierType;
    };
    bool m_loop;
    float m_t;                           // current time
    int m_i;                            // index to the current quat
    std::vector<KeyFrame> m_keyFrames;
    float m_z;
};


class MoveDynamics : public Component {
public:
    MoveDynamics(float);
    void update(double) override;
    void addElasticForce(float ox, float oy, float oz, float k);
    void setConstantForce(float fx, float fy, float fz);
    void setVelocity(float vx, float vy, float vz);
    void setMinY(float);
    void setMaxY(float);
    void setCallback(pybind11::function f);
private:
    glm::vec3 m_elasticCenter;
    glm::vec3 m_constantForce;
    float m_k;
    glm::vec3 m_velocity;
    float m_mass;
    float m_inverseMass;
    glm::vec3 m_force;
    glm::vec3 m_min;
    glm::vec3 m_max;
    pybind11::function m_func;
};


inline void MoveDynamics::setCallback(pybind11::function f) {
    m_func = f;
}
