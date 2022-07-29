#include "collisionresponse.h"
#include "../pyhelper.h"
#include "../node.h"

bool CollisionResponseManager::hasCollision(Collider * a, Collider * b) {
	int at = a->getCollisionTag();
	int bt = b->getCollisionTag();
	return m_response.count(std::make_pair(at, bt)) > 0 || m_response.count(std::make_pair(bt, at));
}


void CollisionResponseManager::add(int i , int j, const pybind11::kwargs& f) {

	CollisionResponse resp;
	resp.onStart = dictget(f, "on_start", pybind11::function());
	resp.onStay = dictget(f, "on_stay", pybind11::function());
	resp.onEnd = dictget(f, "on_end", pybind11::function());
	m_response[std::make_pair(i, j)] = resp;
}

void CollisionResponseManager::onStart(Collider * c1, Collider * c2, glm::vec3 v) {
	auto handler = m_response.find(std::make_pair(c1->getCollisionTag(), c2->getCollisionTag()));
	if (handler != m_response.end() && handler->second.onStart) {
		handler->second.onStart(c1->getNode(), c2->getNode(), v);
	} else {
		handler = m_response.find(std::make_pair(c2->getCollisionTag(), c1->getCollisionTag()));
		if (handler != m_response.end() && handler->second.onStart) {
			handler->second.onStart(c2->getNode(), c1->getNode(), -v);
		}
	}
}

void CollisionResponseManager::onStay(Collider * c1, Collider * c2) {
	auto handler = m_response.find(std::make_pair(c1->getCollisionTag(), c2->getCollisionTag()));
	if (handler != m_response.end() && handler->second.onStay) {
		handler->second.onStay(c1->getNode(), c2->getNode());
	} else {
		handler = m_response.find(std::make_pair(c2->getCollisionTag(), c1->getCollisionTag()));
		if (handler != m_response.end() && handler->second.onStay) {
			handler->second.onStay(c2->getNode(), c1->getNode());
		}
	}
}

void CollisionResponseManager::onEnd(Collider * c1, Collider * c2) {
	auto handler = m_response.find(std::make_pair(c1->getCollisionTag(), c2->getCollisionTag()));
	if (handler != m_response.end() && handler->second.onEnd) {
		handler->second.onEnd(c1->getNode(), c2->getNode());
	} else {
		handler = m_response.find(std::make_pair(c2->getCollisionTag(), c1->getCollisionTag()));
		if (handler != m_response.end() && handler->second.onEnd) {
			handler->second.onEnd(c2->getNode(), c1->getNode());
		}
	}
}