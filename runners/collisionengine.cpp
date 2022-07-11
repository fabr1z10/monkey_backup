#include <iostream>
#include "collisionengine.h"
#include "../node.h"
#include "../shapes/convexpoly.h"

CollisionEngine::CollisionEngine(float width, float height) : m_size(width, height, 0.0f) {}

void CollisionEngine::add(Collider * c) {
	// this is called when a new collider starts. It registers with the engine
	// get the shape bounding box, transform it, map it
	auto aabb = c->getStaticBounds();
	if (!aabb.isVoid()) {
		auto loc = getLocation(aabb);
		pushCollider(c, loc.first, loc.second);
	}
}

void CollisionEngine::move(Collider * c) {
	// TODO mark collider as DIRTY, and update collider position (cells occupied)
	// then at update time, go thorugh each dirty collider, and test it with other colliders in cells
	// to avoid double testing, keep track of tested pairs
	auto aabb = c->getStaticBounds();
	std::cout << "aabb: (" << aabb.min.x << ", " << aabb.min.y << "), (" << aabb.max.x << ", " << aabb.max.y << ")\n";
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
				auto b1 = c1->getStaticBounds();
				auto b2 = c2->getStaticBounds();

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

RayCastHit CollisionEngine::rayCast(glm::vec3 rayOrigin, glm::vec3 rayDir, float length, int mask) {
	glm::vec3 P = rayOrigin;
	glm::vec3 P1 = P;
	float z = rayOrigin.z;

	// initialize current cell
	int i = getIndex(P.x, m_size[0]);
	int j = getIndex(P.y, m_size[1]);
	//int k = (m_3d ? static_cast<int>(P.z / m_size.z) : 0);

	int n = (rayDir.x > 0 ? 1 : 0);
	int m = (rayDir.y > 0 ? 1 : 0);

	float l = 0.0f;
	bool endReached = false;
	int id = 0, jd = 0, kd = 0;
	RayCastHit out;
	out.length = length;

	// we can (and we MUST) exit the loop as soon as we find a collision
	while (!endReached && !out.collide) {
		// get the next point into this cell
		// compute how much distance you need to cover to hit the cell boundary,
		// and what boundary you hit first (x, y or z)
		float tx = (rayDir.x == 0.0f) ? std::numeric_limits<float>::infinity() : ((i+n) * m_size.x - P.x) / rayDir.x;
		float ty = (rayDir.y == 0.0f) ? std::numeric_limits<float>::infinity() : ((j+m) * m_size.y - P.y) / rayDir.y;
		float tm {0.0f};
		id = 0;
		jd = 0;
		if (tx <= ty) {
			tm = tx;
			id = rayDir.x > 0 ? 1 : -1;
		} else {
			tm = ty;
			jd = rayDir.y > 0 ? 1 : -1;
		}

		// advance by tm
		if (l + tm < length) {
			// need to add a tiny extra bit in case the colliding object is a line that lies exactly at the border
			// of two neighboring cell!
			P1 = P + (tm+0.01f) * rayDir;
			// add tm to the cumulated length done
			l += tm;
		} else {
			P1 = P + (length - l) * rayDir;
			endReached = true;
		}

		// get the colliders at the current cell
		auto it = m_cells.find(glm::ivec3(i, j, 0));
		Segment line(P.x, P.y, P1.x, P1.y);
		auto lineBounds = line.getBounds();
		if (it != m_cells.end()) {
			for (auto& c : it->second.colliders) {
				if (!c->isActive())
					continue;
				// aabb check
				int flag = c->getCollisionFlag();
				int fm = flag & mask;
				if (fm != 0) {
					auto shapeBounds = c->getStaticBounds();
					if (lineBounds.intersect2D(shapeBounds)) {
						const auto& t = c->getNode()->getWorldMatrix();
						// if aabb intersect, then try to run proper intersection between the shapes (one of which is a seg)
						/// TODO restore following code
//						auto report = m_raycast->run(rayOrigin, rayDir, length, c->GetShape(), t);
//						if (report.collide && (!out.collide || out.length > report.length)) {
//							out.entity = c;
//							out.length = report.length;
//							out.collide = true;
//							out.normal = report.normal;
//						}
					}
				}
			}
		}
		P = P1;
		i += id;
		j += jd;
	}

	return out;


}