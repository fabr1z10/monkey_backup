#pragma once

#include "statemachine.h"
#include "controller.h"
#include "dynamics.h"
#include "renderer.h"


class Attack : public State {
public:
    Attack(const std::string& id, const pybind11::kwargs&);
    void init(const pybind11::kwargs& args) override;
    void run(double) override;
    void end() override;
    void setParent(StateMachine*) override;
    void keyCallback(int key) override;

private:
    // the animations to play
    std::vector<std::string> m_anims;
    Node* m_node;
    Controller2D* m_controller;
    Dynamics* m_dynamics;
    std::string m_exitState;
    float m_gravity;
    float m_acceleration;
    float m_maxSpeed;
    float m_accelerationTime;
    // index to the animation to play
    int m_currentAnim;
    AnimatedRenderer* m_renderer;

};