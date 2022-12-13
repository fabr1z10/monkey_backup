#include "keypad.h"
#include "../pyhelper.h"
#include <GLFW/glfw3.h>

extern GLFWwindow* window;

KeyPad::KeyPad(const pybind11::kwargs& args) {
	// default is jump with btn 1
	m_jumpKey = dictget<unsigned>(args, "jump_key", 0x10);
}

AIKeyPad::AIKeyPad(const pybind11::kwargs& args) : KeyPad(args) {}

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
}

std::type_index UserKeyPad::getType() {
	return std::type_index(typeid(KeyPad));
}

std::type_index AIKeyPad::getType() {
	return std::type_index(typeid(KeyPad));
}

