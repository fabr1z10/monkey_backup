#include "hotspotmanager.h"
#include "../node.h"

void HotSpotManager::start() {
    m_camera = dynamic_cast<OrthoCamera*>(m_node->getCamera().get());
    assert(m_camera != nullptr);
    m_previous = nullptr;
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
    }


}

void HotSpotManager::add(HotSpot * h) {
    m_hotspot.insert(h);
}

void HotSpotManager::remove(HotSpot * h) {
    m_hotspot.erase(h);
}