#pragma once

#include "../components/statemachine.h"
#include "../components/controller.h"
#include "../components/dynamics.h"
#include "../components/renderer.h"

class Node;

class Hit2D : public State {
public:
    Hit2D(const std::string& id, const pybind11::kwargs&);
    void init(const pybind11::kwargs& args) override;
    void run(double) override;
    void setParent(StateMachine*) override;

private:
    float m_speed;
    std::string m_anim;
    float m_actualSpeed;
    Controller2D* m_controller;
    Dynamics* m_dynamics;
    float m_gravity;
    AnimatedRenderer* m_animatedRenderer;
    float m_timeToStop;
    float m_ax;
    std::string m_exitState;
};