#include "panzoom.h"
#include "../node.h"
#include <GLFW/glfw3.h>

void PanZoom::start() {
	m_camera = dynamic_cast<OrthoCamera*>(m_node->getCamera().get());
	assert(m_camera != nullptr);
}


void PanZoom::cursorPosCallback(GLFWwindow * window, double x, double y) {

	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS)
	{
		auto dx = x - m_lastX;
		auto dy = y - m_lastY;
		m_camera->move(glm::vec2(-dx, dy));
		m_lastX = x;
		m_lastY = y;
	}
}

void PanZoom::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

		glfwGetCursorPos(window, &m_lastX, &m_lastY);


	}
}