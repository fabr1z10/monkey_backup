#pragma once

#include "walk2d.h"
#include "../components/controller.h"
#include "../components/dynamics.h"
#include "../components/renderer.h"

class Node;

class Bounce : public State {
public:
    Bounce(const std::string& id, const pybind11::kwargs&);
    void run(double) override;
    void init(const pybind11::kwargs& args) override;
    void setParent(StateMachine*) override;


private:
    Controller2D* m_controller;
    Dynamics* m_dynamics;
    float m_gravity;
    Node* m_node;
    pybind11::function m_onBounceY;
    int m_bounceCountY;
    float m_fixedVelocityBounce;
    bool m_isFixedVel;
    bool m_checkWalls;
    AnimatedRenderer* m_animatedRenderer;
    std::string m_animation;
};


