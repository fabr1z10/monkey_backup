#include "intersector.h"
#include "../shapes/convexpoly.h"
#include "../shapes/circle.h"
#include "../util.h"


Intersector2D::Intersector2D() {

	add<ConvexPoly, ConvexPoly>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
	add<Rect, ConvexPoly>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
	add<Segment, ConvexPoly>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
	add<Rect, Rect>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
	add<Rect, Segment>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
	add<Segment, Segment>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });

    add<ConvexPoly, Circle>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATCircle(s1, s2, t1, t2); });
    add<Rect, Circle>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATCircle(s1, s2, t1, t2); });
    add<Segment, Circle>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATCircle(s1, s2, t1, t2); });


}


CollisionReport Intersector2D::SAT(const Shape * s1, const Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
	const auto* cp1 = static_cast<const ConvexPoly*>(s1);
	const auto* cp2 = static_cast<const ConvexPoly*>(s2);
	std::vector<glm::vec2> axes;
	for (const auto& t : cp1->getUnitNormals()) axes.push_back(t1 * glm::vec4(t, 0.f, 0.f));
	for (const auto& t : cp2->getUnitNormals()) axes.push_back(t2 * glm::vec4(t, 0.f, 0.f));
	//axes.insert(axes.end(), cp2->getUnitNormals().begin(), cp2->getUnitNormals().end());
	return performSAT(axes, cp1, cp2, t1, t2);
}

CollisionReport Intersector2D::SATCircle(const Shape * s1, const Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
    const auto* cp1 = static_cast<const ConvexPoly*>(s1);
    const auto* sh2 = static_cast<const Shape2D*>(s2);

    // get circle center in world coordinates
    glm::vec2 centerWorld = t2 * glm::vec4(s2->getOffset(), 1.0f);
    float mind2 = std::numeric_limits<float>::infinity();
    glm::vec2 circleAxis(0.0f);
    for (const auto& p : cp1->getPoints()) {
        glm::vec2 pw = t1 * glm::vec4(p, 0.0f, 1.0f);
        auto d = centerWorld - pw;
        float d2 = d.x * d.x + d.y * d.y;
        if (d2 <= mind2) {
            mind2 = d2;
            circleAxis = d;
        }
    }
    std::vector<glm::vec2> axes;
    axes.push_back(glm::normalize(circleAxis));

    for (const auto& t : cp1->getUnitNormals()) axes.push_back(t1 * glm::vec4(t, 0.f, 0.f));
    return performSAT(axes, cp1, sh2, t1, t2);

}

CollisionReport Intersector2D::performSAT(const std::vector<glm::vec2>& axes, const Shape2D* a, const Shape2D* b, const glm::mat4& t1, const glm::mat4& t2) {
	// axes are given in world coordinates
	// since dot products do not depend on coordinate systems, we transform axes from world to local
	CollisionReport report;
	report.distance = std::numeric_limits<float>::infinity();
	for (auto& axis : axes) {
		glm::vec2 pA = a->project(axis, t1);
		glm::vec2 pB = b->project(axis, t2);
		if (pA.x > pB.y || pB.x > pA.y) {
			// we found a non-overlapping axis
			// so we are sure that the two shapes do not collide
			report.collide = false;
			return report;
		}
		// there's overlapping in this axis
		// compute overlap
		float overlap = computeOverlap(pA, pB);
		if (fabs(overlap) < fabs(report.distance)) {
			report.distance = fabs(overlap);
			report.direction = glm::vec3(axis * (overlap < 0 ? -1.0f : 1.0f), 0.f);
		}
	}
	// if we get here, it means we haven't found a separating axis.
	// Therefore, we can conclude the two shapes collide
	report.collide = true;
	return report;
}

CollisionReport Intersector::intersect(Shape * s1, Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
	auto it = m_functionMap.find(std::make_pair(s1->get_type_index(), s2->get_type_index()));
	if (it == m_functionMap.end()) {
		return CollisionReport();
	}
	return it->second(s1, s2, t1, t2);
}