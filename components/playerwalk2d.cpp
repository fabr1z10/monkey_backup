#include "playerwalk2d.h"
#include <GLFW/glfw3.h>

extern GLFWwindow* window;

PlayerWalk2D::PlayerWalk2D(const std::string& id, const pybind11::kwargs& args) : Walk2D(id, args) {}

void PlayerWalk2D::control() {
    m_left = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
    m_right = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
    m_up = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
}

void PlayerWalk2D::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
    if (m_current && m_spriteRenderer && action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_LEFT:
                m_spriteRenderer->flipHorizontal(true);
                break;
            case GLFW_KEY_RIGHT:
                m_spriteRenderer->flipHorizontal(false);
                break;
        }
    }
}
