#include <iostream>
#include "raycast3d.h"

#include "aabb3d.h"
#include "../../util.h"
#include "prism.h"


RayCaster3D::RayCaster3D() {
	m_functionMap[std::type_index(typeid(AABB3D))] = [&] (const glm::vec3& A, const glm::vec3& B, const Shape* s, const glm::mat4& t) {
		return raycastAABB(A, B, s, t);
	};
    m_functionMap[std::type_index(typeid(Prism))] = [&] (const glm::vec3& A, const glm::vec3& B, const Shape* s, const glm::mat4& t) {
        return raycastPrism(A, B, s, t);
    };
};

RayCastHit RayCaster3D::raycastPrism(const glm::vec3 &A, const glm::vec3 &B, const Shape *s, const glm::mat4 &t) {
	RayCastHit output;
	// ray can be vertical or horizontal
	glm::vec3 dir = B - A;
	const auto* prism = static_cast<const Prism*>(s);
	if (isZero(dir.y)) {
		// horizontal
		std::cout << A.z << ", " << B.z << "\n";
		if (A.z < 2.f && B.z > 2.f) {
		    std::cout << "qui nano\n";
		}
		const auto& edges = prism->getEdges();
		for (const auto& edge : edges) {
			if (edge.mode == 1 && A.y > edge.y0) {
				continue; // no collision here
			}
			float t = 0.f;
			if (seg2seg(glm::vec2(A.x, A.z), glm::vec2(B.x, B.z), edge.A, edge.B, t)) {
				output.collide = true;
				output.length = t ;//* glm::length(B-A);
				return output;
			}
		}
	} else {
		// vertical
		float yPrism = t[3][1];
		// check whether ray is intersecting surface
		if ((B.y - yPrism) * (A.y - yPrism) <= 0.f) {
			// find intersection point
			auto t = -A.y / (B.y - A.y);
			glm::vec3 P = A + t * (B - A);
			if (pnpoly(prism->getPoints(), glm::vec2(P.x, P.z))) {
				// intersection
				output.collide = true;
				output.length = t ;//* glm::length(B - A);
			} else {
				std::cout << "azzo\n";
			}
		}
	}
	return output;
}

RayCastHit RayCaster3D::raycastAABB(const glm::vec3 &A, const glm::vec3 &B, const Shape *s, const glm::mat4 &t) {

	RayCastHit hit;

	// get the transformed bounds
	auto b = s->getBounds();
	b.transform(t);

	glm::vec3 dir = (B - A);

	float tmin = (b.min.x - A.x) / dir.x;
	float tmax = (b.max.x - A.x) / dir.x;

	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (b.min.y - A.y) / dir.y;
	float tymax = (b.max.y - A.y) / dir.y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return hit;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (b.min.z - A.z) / dir.z;
	float tzmax = (b.max.z - A.z) / dir.z;

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return hit;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	hit.collide = true;
	hit.length = tmin;
	return hit;
}