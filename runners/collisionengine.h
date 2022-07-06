#pragma once

#include <unordered_map>
#include <unordered_set>
#include "../runner.h"
#include "../components/collider.h"
#include "../hashpair.h"
#include "../shapes/intersector.h"
#include "collisionresponse.h"


struct CollisionEngineCell {
	bool dirty;
	std::unordered_set<Collider*> colliders;
};

struct CollisionInfo {
	CollisionReport report;
	glm::ivec3 pos;
};

class CollisionEngine : public Runner {
public:
	CollisionEngine(float, float);
	virtual void add (Collider*);
	virtual void move (Collider*) ;
	void update(double) override;

	std::pair<glm::ivec3, glm::ivec3> getLocation(const Bounds& b);
private:
	std::unordered_map<glm::ivec3, CollisionEngineCell> m_cells;
	std::unordered_map<Collider*, std::pair<glm::ivec3, glm::ivec3>> m_colliderLocations;
	void pushCollider(Collider* c, glm::ivec3 m, glm::ivec3 M);
	int getIndex(float, float);
	glm::vec3 m_size;

	// TODO add intersector --> that is the object that does the actual shape intersection
	std::shared_ptr<Intersector> m_intersector;

	// TODO add response manager --> object that holds the callback for collision
	std::shared_ptr<CollisionResponse> m_responseManager;
};
