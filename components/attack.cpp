#include "attack.h"
#include "../node.h"
#include <GLFW/glfw3.h>

Attack::Attack(const std::string& id, const pybind11::kwargs& args) : State(id, args) {
    for (const auto& anim : args["anims"])
        m_anims.push_back(anim.cast<std::string>());
    //m_anims = args["anims"].cast<std::vector<std::string>>();
    m_exitState = args["exit_state"].cast<std::string>();

}

void Attack::init(const pybind11::kwargs& args) {
//    auto delta = duration_cast<milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - m_lastHit;
//    if (delta > 500) {
//        // reset
//        m_currentAnim = 0;
//    }
    m_renderer->setAnimation(m_anims[m_currentAnim]);
}

void Attack::keyCallback(int key) {
    if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
        m_node->setFlipX(key == GLFW_KEY_LEFT);
        m_sm->setState(m_exitState);
    }
    State::keyCallback(key);
}

void Attack::end() {}

void Attack::setParent(StateMachine * sm) {

    State::setParent(sm);
    m_renderer = dynamic_cast<AnimatedRenderer*>(sm->getNode()->getComponent<Renderer>());
    m_node = sm->getNode();

}

void Attack::run(double) {
    if (/*m_changeStateAfterAnim &&*/m_renderer->isComplete()) {
        m_sm->setState(m_exitState);

    }


}