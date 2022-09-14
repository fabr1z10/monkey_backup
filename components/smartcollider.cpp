#include "smartcollider.h"
#include "../node.h"
#include "../engine.h"
#include "../pyhelper.h"

#include "../models/sprite.h"

SpriteCollider::SpriteCollider(int flag, int mask, int tag) : Collider(),
m_sprite(nullptr), m_renderer(nullptr), m_flag(flag), m_mask(mask), m_tag(tag) {}

void SpriteCollider::start() {
    // a node that has a spritecollider needs to have a sprite model (and a sprite renderer)
    m_renderer = dynamic_cast<SpriteRenderer*>(m_node->getComponent<Renderer>());
    assert(m_renderer != nullptr);

    m_sprite = m_renderer->getSprite();
    m_staticBounds = m_sprite->getStaticBounds();
    Collider::start();
//    auto& engine = Engine::instance();
//    auto collDebug = pyget<bool>(engine.getConfig().attr("settings"),"debug_collision", false);
//    if (collDebug) {
//        generateDebugMesh();
//    }

}

std::shared_ptr<Shape> SpriteCollider::getShape() {
    return m_sprite->getShape(m_renderer->getAnimation(), m_renderer->getFrame());
}

int SpriteCollider::getCollisionTag() const {
    return m_tag;
}

int SpriteCollider::getCollisionFlag() const {
    return m_sprite->hasCollision(m_renderer->getAnimation()) ? m_flag : 0;
}

int SpriteCollider::getCollisionMask() const {
    return m_sprite->hasCollision(m_renderer->getAnimation()) ? m_mask : 0;
}

void SpriteCollider::generateDebugMesh() {
	if (m_debugNode != nullptr) {
		m_debugNode->remove();
	}
    auto model = m_sprite->generateDebugModel();
    auto node = std::make_shared<Node>();
    node->setModel(model);
    m_node->add(node);
    m_debugNode = node.get();
}