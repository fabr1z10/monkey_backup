#include "attack.h"
#include "../node.h"
#include <GLFW/glfw3.h>
#include "../pyhelper.h"
#include "../util.h"

extern GLFWwindow* window;


Attack::Attack(const std::string& id, const pybind11::kwargs& args) : State(id, args), m_currentAnim(0) {
    for (const auto& anim : args["anims"])
        m_anims.push_back(anim.cast<std::string>());
    //m_anims = args["anims"].cast<std::vector<std::string>>();
    m_exitState = args["exit_state"].cast<std::string>();
    m_maxSpeed = args["speed"].cast<float>();
    m_gravity = dictget<float>(args, "gravity", 0.0f);
    m_accelerationTime = dictget<float>(args, "acc_time", 0.1f);
    m_acceleration = m_maxSpeed / m_accelerationTime;
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
    if (m_controller->grounded() && (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT)) {
        m_sm->setState(m_exitState);
    }
    m_node->setFlipX(key == GLFW_KEY_LEFT);

    State::keyCallback(key);
}

void Attack::end() {}

void Attack::setParent(StateMachine * sm) {

    State::setParent(sm);
    m_renderer = dynamic_cast<AnimatedRenderer*>(sm->getNode()->getComponent<Renderer>());
    m_node = sm->getNode();
    m_controller = dynamic_cast<Controller2D*>(m_sm->getNode()->getComponent<Controller>());
    assert(m_controller != nullptr);

    m_dynamics = m_sm->getNode()->getComponent<Dynamics>();
    assert(m_dynamics != nullptr);
}

void Attack::run(double dt) {
    if (!m_controller->grounded()) {
        auto dtf = static_cast<float>(dt);
        if (m_controller->ceiling()) {
            m_dynamics->m_velocity.y = 0;
        }
        glm::vec3 a(0.0f);
        auto left = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
        auto right = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
        a.y = -m_gravity;

        if (left || right) {
            a.x = m_acceleration; // (m_left ? -1.f : 1.f) * m_acceleration;
        } else {
            // apply deceleration only if velocity above threshold
            if (fabs(m_dynamics->m_velocity.x) > 0.1f) {
                a.x = - m_acceleration;
            } else {
                a.x = 0.0f;
                m_dynamics->m_velocity.x = 0.0f;
            }
        }
        m_dynamics->m_velocity += a * dtf;
        // limit horizontal vel to max speed
        if (left || right) {
            if (fabs(m_dynamics->m_velocity.x) > m_maxSpeed) {
                m_dynamics->m_velocity.x = signf(m_dynamics->m_velocity.x) * m_maxSpeed;
            }
        }


        auto delta = m_dynamics->m_velocity * dtf;

        m_controller->move(delta, false);

    }








    if (/*m_changeStateAfterAnim &&*/m_renderer->isComplete()) {
        m_sm->setState(m_exitState);

    }


}