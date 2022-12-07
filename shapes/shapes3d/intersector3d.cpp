#include "intersector3d.h"
#include "aabb3d.h"

Intersector3D::Intersector3D() {

	add<AABB3D, AABB3D>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return AABB2(s1, s2, t1, t2); });

}

CollisionReport Intersector3D::AABB2(const Shape * s1, const Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
	auto b1 = s1->getBounds();
	auto b2 = s2->getBounds();

	b1.transform(t1);
	b2.transform(t2);

	CollisionReport report;
	report.collide = !(b1.min.x > b2.max.x || b2.min.x > b1.max.x || b1.min.y > b2.max.y || b2.min.y > b1.max.y || b1.min.z > b2.max.z || b2.min.x > b1.max.z);
	return report;
}
