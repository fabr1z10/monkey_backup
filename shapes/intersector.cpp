#include "intersector.h"
#include "../shapes/convexpoly.h"
#include "../shapes/circle.h"
#include "../util.h"
#include "../shapes/compound.h"
#include "aabb.h"


Intersector2D::Intersector2D() {

	add<ConvexPoly, ConvexPoly>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
	add<Rect, ConvexPoly>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
	add<Segment, ConvexPoly>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
	add<Rect, Rect>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
	add<Rect, Segment>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });
	add<Segment, Segment>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SAT(s1, s2, t1, t2); });

	// circle
    add<ConvexPoly, Circle>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATCircle(s1, s2, t1, t2); });
    add<Rect, Circle>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATCircle(s1, s2, t1, t2); });
    add<Segment, Circle>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATCircle(s1, s2, t1, t2); });

    add<CompoundShape, ConvexPoly> ([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return compound(s1, s2, t1, t2); });
    add<CompoundShape, Rect> ([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return compound(s1, s2, t1, t2); });
    add<CompoundShape, Segment> ([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return compound(s1, s2, t1, t2); });
    add<CompoundShape, Circle> ([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return compound(s1, s2, t1, t2); });
    add<CompoundShape, AABB> ([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return compound(s1, s2, t1, t2); });

    add<AABB, ConvexPoly>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATAABB(s1, s2, t1, t2); });
    add<AABB, Rect>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATAABB(s1, s2, t1, t2); });
    add<AABB, Segment>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATAABB(s1, s2, t1, t2); });
    add<AABB, AABB>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return AABB2(s1, s2, t1, t2); });
}

CollisionReport Intersector2D::compound(const Shape * s1, const Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
    const auto* cs = static_cast<const CompoundShape*>(s1);
    for (const auto& s : cs->getShapes()) {
        auto report = this->intersect(s.get(), s2, t1, t2);
        if (report.collide) {
            return report;
        }
    }
    return CollisionReport();
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

CollisionReport Intersector2D::SATAABB(const Shape * s1, const Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
    const auto* cp1 = static_cast<const Shape2D*>(s1);
    const auto* cp2 = static_cast<const ConvexPoly*>(s2);
    std::vector<glm::vec2> axes { glm::vec2(1.f, 0.f), glm::vec2(0.f, 1.f)};
    for (const auto& t : cp2->getUnitNormals()) axes.push_back(t2 * glm::vec4(t, 0.f, 0.f));
    //axes.insert(axes.end(), cp2->getUnitNormals().begin(), cp2->getUnitNormals().end());
    return performSAT(axes, cp1, cp2, t1, t2);
}

CollisionReport Intersector2D::AABB2(const Shape * s1, const Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
    auto b1 = s1->getBounds();
    auto b2 = s2->getBounds();
    glm::vec3 tr1(t1[3]);
    glm::vec3 tr2(t1[3]);
    auto m1 = b1.min + tr1;
    auto M1 = b1.max + tr1;
    auto m2 = b2.min + tr2;
    auto M2 = b2.max + tr2;
    CollisionReport report;
    bool notCollide =(m2.x > M1.x || M2.x < m1.x) || (m2.y > M2.y || M2.y < m1.y);
    report.collide = !notCollide;
    return report;

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

CollisionReport Intersector::intersect(const Shape * s1, const Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
	auto it = m_functionMap.find(std::make_pair(s1->get_type_index(), s2->get_type_index()));
	if (it == m_functionMap.end()) {
        auto it2 = m_functionMap.find(std::make_pair(s2->get_type_index(), s1->get_type_index()));
        if (it2 == m_functionMap.end()) {
            return CollisionReport();
        }
        return it2->second(s2, s1, t2, t1);
	}

	return it->second(s1, s2, t1, t2);
}