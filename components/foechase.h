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
    glm::vec2 m_chaseRange;
    float m_leftBound;
    float m_rightBound;
};


