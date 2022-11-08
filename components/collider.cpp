#include "collider.h"
#include "../engine.h"
#include "../error.h"
#include "../runners/collisionengine.h"
#include "../pyhelper.h"
#include "../pyfunc.h"
#include "../node.h"

Collider::Collider() : m_callbackHandle(-1), m_engine(nullptr), m_debugNode(nullptr) {
    //std::cout << "creating collider\n";
}

void Collider::start() {
	auto& engine = Engine::instance();
	auto collDebug = pyget<bool>(engine.getConfig().attr("settings"),"debug_collision", false);
	auto room = engine.getRoom();
	m_engine = room->getRunner<CollisionEngine>();
	//m_engine = Engine::get().GetRunner<ICollisionEngine>();
	if (m_engine == nullptr) {

		//GLIB_FAIL("The room has a collider component but no collision engine is loaded.");
	} else {
		m_engine->add(this);
		m_callbackHandle = m_node->onMove.reg([&] (Node* e) { m_engine->move(this); } );
	}


	// register to move. When the object moves, we notify the collision engine

	if (collDebug) {
		generateDebugMesh();
	}
}

Bounds Collider::getStaticBounds() const {
	auto b = m_staticBounds;
	b.transform(m_node->getWorldMatrix());
	return b;

}

glm::vec4 Collider::bounds() const {
    auto b = getStaticBounds();
    return glm::vec4(b.min.x, b.min.y, b.max.x, b.max.y);
}

std::type_index Collider::getType() {
    return std::type_index(typeid(Collider));
}

Collider::~Collider() {
    //std::cout << "destroying collider\n";
    if (m_engine != nullptr)
        m_engine->remove(this);
	if (m_callbackHandle != -1)
		m_node->onMove.unreg(m_callbackHandle);
}

SimpleCollider::SimpleCollider(std::shared_ptr<Shape> shape, int flag, int mask, int tag) : m_shape(shape), m_flag(flag),
 m_mask(mask), m_tag(tag) {
	m_staticBounds = m_shape->getBounds();
}

void SimpleCollider::generateDebugMesh() {
	if (m_debugNode != nullptr) {
		m_debugNode->remove();
	}
	auto args = pybind11::kwargs();
	args["z"] =1.f;
	auto model = makeModel(m_shape, args);
	auto node = std::make_shared<Node>();
	node->setModel(model);
	m_node->add(node);
	m_debugNode = node.get();
}