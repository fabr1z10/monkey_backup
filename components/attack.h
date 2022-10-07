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

    std::string m_exitState;

    // index to the animation to play
    int m_currentAnim;
    AnimatedRenderer* m_renderer;

};