#pragma once

#include "walk2d.h"
#include "controller.h"
#include "dynamics.h"
#include "renderer.h"

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


class FoeChase2D : public Walk2D {
public:
    FoeChase2D(const std::string& id, const pybind11::kwargs&);
    void control() override {}
    void setParent(StateMachine*) override;
    void run(double) override;
private:
    bool randomAttack();
    struct AttackDetails {
        std::string attackId;
        bool inRange;               // attacks only if within range
    };

    bool m_flipOnEdge;
    bool m_flipHorizontally;
    bool m_inRange;
    int m_targetId;
    Node* m_target;
    float m_attackDistance;
    float m_maxSpeed;
    glm::vec2 m_attackRange;
    std::map<float, AttackDetails> m_attackMap;

};


