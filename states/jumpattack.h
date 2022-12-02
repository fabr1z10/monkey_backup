#pragma once

#include "../components/statemachine.h"
#include "../components/controller.h"
#include "../components/dynamics.h"
#include "../components/renderer.h"

class Node;

class JumpAttack : public State {
public:
    JumpAttack(const std::string& id, const pybind11::kwargs&);
    void init(const pybind11::kwargs& args) override;
    void run(double) override;
    void setParent(StateMachine*) override;

private:
    int m_targetId;
    CollisionEngine* m_engine;
    std::string m_anim;
    float m_actualSpeed;
    Controller2D* m_controller;
    Dynamics* m_dynamics;
    float m_gravity;
    AnimatedRenderer* m_animatedRenderer;
    float m_timeToStop;
    float m_ax;
    std::string m_exitState;
    std::string m_jumpUp;
    std::string m_jumpDown;
    float m_downSpeed;
    int m_castMask;
    int m_castTag;
    Node* m_target;
    float m_peakHeight;
    bool m_hit;

    std::string m_hitState;

};