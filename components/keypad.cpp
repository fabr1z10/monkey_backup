#include "keypad.h"
#include "../pyhelper.h"

#include "../engine.h"

#include "renderer.h"
#include "../random.h"

extern GLFWwindow* window;

KeyPad::KeyPad(const pybind11::kwargs& args) {
	// default is jump with btn 1
	m_jumpKey = dictget<unsigned>(args, "jump_key", 0x10);
}

AIKeyPad::AIKeyPad(const pybind11::kwargs& args) : KeyPad(args) {
    m_targetId = dictget<int>(args, "target", -1);
    m_attackProbability = 0.01;

}

void AIKeyPad::start() {
    if (m_targetId != -1) {
        m_target = Engine::instance().getNode(m_targetId).get();
        auto renderer = dynamic_cast<AnimatedRenderer*>(m_node->getComponent<Renderer>());
        auto range = renderer->getAttackRange();
        m_attackRange = glm::vec2(range.min.x, range.max.x);
        m_targetDistance = 0.5f * (m_attackRange.x + m_attackRange.y);
    }
}

UserKeyPad::UserKeyPad(const pybind11::kwargs& args) : KeyPad(args) {

	m_leftKey = dictget<unsigned>(args, "left", GLFW_KEY_LEFT);
	m_rightKey = dictget<unsigned>(args, "left", GLFW_KEY_RIGHT);
	m_upKey = dictget<unsigned>(args, "left", GLFW_KEY_UP);
	m_downKey = dictget<unsigned>(args, "left", GLFW_KEY_DOWN);
	m_btn1 = dictget<unsigned>(args, "left", GLFW_KEY_A);
	m_btn2 = dictget<unsigned>(args, "left", GLFW_KEY_S);
	m_btn3 = dictget<unsigned>(args, "left", GLFW_KEY_Q);
	m_btn4 = dictget<unsigned>(args, "left", GLFW_KEY_W);

}
void KeyPad::update(double) {
	// update flip hor
	m_fliph = left() ? true : (right() ? false : m_fliph);
}

void UserKeyPad::update(double dt) {
	m_keys = 0u;
	m_back = 1;
	m_keys |= (glfwGetKey(window, m_leftKey) == GLFW_PRESS ? 0x01u : 0u);
	m_keys |= (glfwGetKey(window, m_rightKey) == GLFW_PRESS ? 0x02u : 0u);
	m_keys |= (glfwGetKey(window, m_upKey) == GLFW_PRESS ? 0x04 : 0u);
	m_keys |= (glfwGetKey(window, m_downKey) == GLFW_PRESS ? 0x08 : 0u);
	m_keys |= (glfwGetKey(window, m_btn1) == GLFW_PRESS ? 0x10 : 0u);
	m_keys |= (glfwGetKey(window, m_btn2) == GLFW_PRESS ? 0x20 : 0u);
	m_keys |= (glfwGetKey(window, m_btn3) == GLFW_PRESS ? 0x40 : 0u);
	m_keys |= (glfwGetKey(window, m_btn4) == GLFW_PRESS ? 0x80 : 0u);
	KeyPad::update(dt);
}



void AIKeyPad::update(double dt) {
	m_keys= 0u;
	m_back = 1;
    auto targetPos = m_target->getWorldPosition();
    auto entityPos = m_node->getWorldPosition();
    m_fliph = targetPos.x < entityPos.x;

    auto scale = m_node->getScale();
    float scaledDistance = scale * m_targetDistance;
    if (entityPos.x >= targetPos.x) {
        targetPos.x += scaledDistance;
    } else {
        targetPos.x -= scaledDistance;
    }
    auto delta = targetPos - entityPos;
    delta.y = 0.f;
    std::cout << "(" << delta.x << ", " << delta.z << ")\n";
    // if within range, stop
    if (glm::length(delta) <= 0.1f) {
        // attack or do nothing
        float u = Random::instance().getUniformReal(0.0f, 1.0f);
        if (u < m_attackProbability) {
            // push random btn
            m_keys |= 0x20;
        }
    } else {
        if (fabs(delta.x) > 0.1f) {
            if (delta.x > 0) {
                m_keys |= 0x02u;
                if (m_fliph) {
                    m_back = -1;
                }
            } else {
                m_keys |= 0x01u;
                if (!m_fliph) {
                    m_back = -1;
                }
            }
        }
        if (fabs(delta.z) > 0.1f) {
            m_keys |= (delta.z > 0 ? 0x08u : 0x04u);
        }
    }


}

std::type_index UserKeyPad::getType() {
	return std::type_index(typeid(KeyPad));
}

std::type_index AIKeyPad::getType() {
	return std::type_index(typeid(KeyPad));
}

