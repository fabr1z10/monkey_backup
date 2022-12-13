//#include "playerwalk3d.h"
//#include "../node.h"
//#include <GLFW/glfw3.h>
//
//extern GLFWwindow* window;
//
//PlayerWalk3D::PlayerWalk3D(const std::string& id, const pybind11::kwargs& args) : Walk3D(id, args) {}
//
//void PlayerWalk3D::control() {
//	m_keys = 0u;
//	m_keys |= (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS ? 0x01u : 0u);
//	m_keys |= (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS ? 0x02u : 0u);
//	m_keys |= (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ? 0x04 : 0u);
//	m_keys |= (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ? 0x08 : 0u);
//	m_keys |= (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ? 0x10 : 0u);
//
//}
//
//void PlayerWalk3D::keyCallback(int key) {
//	switch (key) {
//		case GLFW_KEY_LEFT:
//			m_node->setFlipX(true);
//			break;
//		case GLFW_KEY_RIGHT:
//			m_node->setFlipX(false);
//			break;
//	}
//	State::keyCallback(key);
//}
