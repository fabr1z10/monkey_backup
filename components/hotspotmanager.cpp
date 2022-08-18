#include "hotspotmanager.h"
#include "../node.h"
#include "../engine.h"
#include <GLFW/glfw3.h>

void HotSpotManager::start() {
    m_camera = dynamic_cast<OrthoCamera*>(m_node->getCamera().get());
    assert(m_camera != nullptr);
    m_previous = nullptr;
}

HotSpotManager::~HotSpotManager() {
	for (const auto& h : m_hotspot) {
		h->removeManager();
	}
}

void HotSpotManager::cursorPosCallback(GLFWwindow *, double x, double y) {
    bool isInViewport = m_camera->isInViewport(x, y);
    if (isInViewport) {

        HotSpot* current = nullptr;
        glm::vec2 worldCoords = m_camera->getWorldCooridnates(x, y);
        for (auto& hotspot : m_hotspot) {
            if (current != nullptr && hotspot->getPriority() <= current->getPriority()) {
                continue;
            }
            if (hotspot->isMouseInside(worldCoords)) {
                current = hotspot;
            }
        }
        if (current != m_previous) {
            if (m_previous != nullptr) {
                m_previous->exit();
            }
            if (current != nullptr) {
                current->enter();
            }
            m_previous = current;

        }


    } else {
        if (m_previous != nullptr) {
            m_previous->exit();
            m_previous = nullptr;
        }
    }


}

void HotSpotManager::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

        if (m_previous != nullptr) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            glm::vec2 worldCoords = m_camera->getWorldCooridnates(xpos, ypos);
            m_previous->click(worldCoords);
        }
    }
}

void HotSpotManager::add(HotSpot * h) {
    m_hotspot.insert(h);
}

void HotSpotManager::remove(HotSpot * h) {
    m_hotspot.erase(h);
}