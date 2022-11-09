#pragma once

#include "walk2d.h"
#include "controller.h"
#include "dynamics.h"
#include "renderer.h"

class Node;

class FoeChase2D : public Walk2D {
public:
    FoeChase2D(const std::string& id, const pybind11::kwargs&);
    void control() override {}
    void setParent(StateMachine*) override;
    void run(double) override;
private:
    bool m_flipOnEdge;
    bool m_flipHorizontally;
    int m_targetId;
    Node* m_target;
    float m_attackDistance;
    float m_maxSpeed;
    glm::vec2 m_attackRange;
};


