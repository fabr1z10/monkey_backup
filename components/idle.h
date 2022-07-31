#pragma once

#include "statemachine.h"
#include "renderer.h"

class Idle : public State {
public:
    Idle(const std::string& id, const std::string& anim, const pybind11::kwargs&);
    void init() override;
    void setParent(StateMachine*) override;
private:
    std::string m_animation;
    SpriteRenderer* m_spriteRenderer;

};


