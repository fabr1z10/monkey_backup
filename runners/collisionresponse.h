#pragma once

#include <pybind11/pybind11.h>
#include "../components/collider.h"

class CollisionResponse {
public:
	bool hasCollision(Collider*, Collider*);

private:
	std::unordered_map<std::pair<int, int>, pybind11::function> m_response;

};

