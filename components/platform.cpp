#include "platform.h"
#include "controller.h"
#include "../node.h"


void Platform::start() {
    m_node->onMove.reg([&](Node* node) { move(node); });
    m_lastPosition = m_node->getWorldPosition();
}

void Platform::unregisterComponent(Controller2D* character) {
    m_characters.erase(character);
}

void Platform::registerComponent(Controller2D* character) {
    m_characters.insert(character);
}

void Platform::unregisterAll() {
    m_characters.clear();
}

void Platform::move(Node* node) {
    glm::vec3 currentPosition = node->getWorldPosition();
    glm::vec3 delta = currentPosition - m_lastPosition;
    if (delta.x != 0.0f || delta.y != 0.0f) {
        for (const auto& c : m_characters) {
//            delta.x *= (node->getFilpX() ? -1.f : 1.f);
            c->move(delta);
            c->setGrounded(true);
        }
    }
    m_lastPosition = currentPosition;
}