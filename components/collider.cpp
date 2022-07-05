#include "collider.h"
#include "../engine.h"
#include "../error.h"
#include "../runners/collisionengine.h"

Collider::Collider() : m_callbackHandle(-1), m_engine(nullptr) {}

void Collider::start() {
	auto room = Engine::instance().getRoom();
	m_engine = room->getRunner<CollisionEngine>();
	//m_engine = Engine::get().GetRunner<ICollisionEngine>();
	if (m_engine == nullptr) {
		GLIB_FAIL("The room has a collider component but no collision engine is loaded.");
	}
	m_engine->add(this);

	// register to move. When the object moves, we notify the collision engine
	m_callbackHandle = m_node->onMove.reg([&] (Node* e) { m_engine->move(this); } );

}

Bounds Collider::getBounds() const {
	return Bounds();
}

Collider::~Collider() {
	if (m_callbackHandle != -1)
		m_node->onMove.unreg(m_callbackHandle);
}

SimpleCollider::SimpleCollider(std::shared_ptr<Shape> shape, int flag, int mask, int tag) : m_shape(shape), m_flag(flag),
 m_mask(mask), m_tag(tag) {
}

