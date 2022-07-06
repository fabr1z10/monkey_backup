#include "collisionengine.h"
#include "../node.h"

CollisionEngine::CollisionEngine(float width, float height) : m_size(width, height, 0.0f) {}

void CollisionEngine::add(Collider * c) {
	// this is called when a new collider starts. It registers with the engine
	// get the shape bounding box, transform it, map it
	auto aabb = c->getBounds();
	if (!aabb.isVoid()) {
		auto loc = getLocation(aabb);
		pushCollider(c, loc.first, loc.second);
	}
}

void CollisionEngine::move(Collider *) {

}

void CollisionEngine::update(double) {

	std::unordered_map<std::pair<Collider*, Collider*>, CollisionInfo> currentlyCollidingPairs;
	std::unordered_set<glm::ivec3> cellsExamined;

	for (auto& c : m_cells) {
		auto &cell = c.second;
		if (!cell.dirty) {
			continue;
		}
		cellsExamined.insert(c.first);
		// skip cells that have less than 2 colliders
		if (cell.colliders.size() < 2) {
			cell.dirty = false;
			continue;
		}

		for (auto iter = cell.colliders.begin(); iter != cell.colliders.end(); ++iter) {
			auto c1 = *iter;
			if (!c1->isActive())
				continue;
			auto iter2 = iter;
			for (++iter2; iter2 != cell.colliders.end(); ++iter2) {
				auto * c2 = *iter2;
				if (!c2->isActive())
					continue;
				if ( ((c1->getCollisionMask() & c2->getCollisionFlag()) == 0) || ((c2->getCollisionMask() & c1->getCollisionFlag()) == 0)) {
					// If at least one mask is hit, then we keep checking the collision.
					// if both mask fail, then we skip to the next collider
					continue;
				}

				// if no response is provided for these tags, then skip it
				if (m_responseManager == nullptr || !m_responseManager->hasCollision(c1, c2)) {
					continue;
				}

				// we have a collision response, so let's calculate collision
				auto b1 = c1->getBounds();
				auto b2 = c2->getBounds();

				// perform a aabb testing
				if (!b1.intersect2D(b2)) {
					continue;
				}

				// get the shape in local coordinates
				const auto& t1 = c1->getNode()->getWorldMatrix();
				const auto& t2 = c2->getNode()->getWorldMatrix();
				//auto s1 = c1->GetShape()->transform(t1);
				//auto s2 = c2->GetShape()->transform(t2);


				// bounding boxes intersect, so let's make a proper collision test
				auto report = m_intersector->intersect(c1->getShape().get(), c2->getShape().get(), t1, t2);
				if (report.collide) {
					CollisionInfo ci;
					ci.report = report;
					ci.pos = c.first;
					currentlyCollidingPairs.insert(std::make_pair(std::make_pair(c1, c2), ci));
				}


			}
			// set cell as not dirty
			cell.dirty = false;
		}
	}

}

void CollisionEngine::pushCollider(Collider* c, glm::ivec3 m, glm::ivec3 M) {
	for (int i = m.x; i <= M.x; ++i) {
		for (int j = m.y; j <= M.y; ++j) {
			auto &cell = m_cells[glm::ivec3(i, j, 0)];
			cell.colliders.insert(c);
			cell.dirty = true;
		}
	}
	m_colliderLocations[c] = std::make_pair(m, M);
}

std::pair<glm::ivec3, glm::ivec3> CollisionEngine::getLocation(const Bounds &b) {
	glm::ivec3 min(0);
	glm::ivec3 max(0);
	min.x = getIndex(b.min.x, m_size[0]);
	min.y = getIndex(b.min.y, m_size[1]);
	max.x = getIndex(b.max.x, m_size[0]);
	max.y = getIndex(b.max.y, m_size[1]);
	return std::make_pair(min, max);
}

int CollisionEngine::getIndex(float x, float s) {
	return -1 * (x < 0) + static_cast<int>(x / s);
}