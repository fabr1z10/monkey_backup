#pragma once

#include "statemachine.h"
#include "controller.h"
#include "dynamics.h"
#include "renderer.h"

class Node;

class Hit : public State {
public:
    Hit(const std::string& id, const pybind11::kwargs&);
    void init(const pybind11::kwargs& args) override;
    void setParent(StateMachine*) override;
    void run(double) override;
protected:
    float m_gravity;
    glm::vec2 m_initialVelocity;
    std::string m_animFly;
    std::string m_animGround;
    Controller2D* m_controller;
    Dynamics* m_dynamics;
    Node* m_node;
    AnimatedRenderer* m_animatedRenderer;
    float m_timer;
    float m_timeout;
    std::string m_exitState;
};


