#include "intersector.h"
#include "../shapes/convexpoly.h"
#include "../shapes/circle.h"
#include "../util.h"
#include "../shapes/compound.h"
#include "aabb.h"
#include "triangles.h"
#include "projection.h"


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
    add<AABB, Triangles>([&] (const Shape* s1, const Shape* s2, const glm::mat4& t1, const glm::mat4& t2) { return SATTriAABB(s1, s2, t1, t2); });
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

    b1.transform(t1);
    b2.transform(t2);

    CollisionReport report;
    report.collide = !(b1.min.x > b2.max.x || b2.min.x > b1.max.x || b1.min.y > b2.max.y || b2.min.y > b1.max.y);
    return report;
//    //glm::vec3 tr1(t1[3]);
//    //glm::vec3 tr2(t2[3]);
//    auto b10 = t1 * glm::vec4(b1.min, 1.f);
//    auto b11 = t1 * glm::vec4(b1.max, 1.f);
//    auto b20 = t2 * glm::vec4(b2.min, 1.f);
//    auto b21 = t2 * glm::vec4(b2.max, 1.f);
//    auto m1 = glm::vec3(std::min(b10.x, b11.x), std::min(b10.y, b11.y), std::min(b10.z, b11.z));
//    auto M1 = glm::vec3(std::max(b10.x, b11.x), std::max(b10.y, b11.y), std::max(b10.z, b11.z));
//    auto m2 = glm::vec3(std::min(b20.x, b21.x), std::min(b20.y, b21.y), std::min(b20.z, b21.z));
//    auto M2 = glm::vec3(std::max(b20.x, b21.x), std::max(b20.y, b21.y), std::max(b20.z, b21.z));
//    CollisionReport report;
//    float overlap_x{0.f};
//    float overlap_y{0.f};
//    if (M1.x < m2.x) {
//        // do nothing
//    } else {
//        if (M1.x < M2.x) {
//            float x1 = fabs(M1.x - m2.x);
//            float x2 = fabs(M2.x - m1.x);
//            overlap_x = (x1 < x2) ? -x1 : x2;
//        } else {
//            if (m1.x < M2.x) {
//                overlap_x = M2.x - m1.x;
//            }
//        }
//    }
//    if (M1.y < m2.y) {
//        // do nothing
//    } else {
//        if (M1.y < M2.y) {
//            float y1 = fabs(M1.y - m2.y);
//            float y2 = fabs(M2.y - m1.y);
//            overlap_y = (y1 < y2) ? -y1 : y2;
//        } else {
//            if (m1.y < M2.y) {
//                overlap_y = M2.y - m1.y;
//            }
//        }
//    }
//    report.collide = overlap_x != 0.f && overlap_y != 0.f;
//    if (report.collide) {
//        if (fabs(overlap_x) > fabs(overlap_y)) {
//            report.direction = glm::vec3(0.f, sign(overlap_y), 0.f);
//            report.distance = fabs(overlap_y);
//        } else {
//            report.direction = glm::vec3(sign(overlap_x), 0.f, 0.f);
//            report.distance = fabs(overlap_x);
//        }
//    }
//
//    //#line  "" notCollide =(m2.x > M1.x || M2.x < m1.x) || (m2.y > M2.y || M2.y < m1.y);
//    //report.collide = !notCollide;
//    return report;

}

CollisionReport Intersector2D::SATTriAABB(const Shape * s1, const Shape * s2, const glm::mat4 & t1, const glm::mat4 & t2) {
    const auto* aabb = static_cast<const AABB*>(s1);
    const auto* trimesh = static_cast<const Triangles*>(s2);
    CollisionReport report;
    report.collide = true;
    auto triDisplacement = glm::vec3(t2[3]);
    // first get the world coordinates of the AABB points
    std::array<glm::vec2, 4> m_aabbPointsWorld;
    const auto& localPoints = aabb->getPoints();
    Projection p_own_aabb_x;
    Projection p_own_aabb_y;
    for (size_t i = 0; i < localPoints.size(); ++i) {
        m_aabbPointsWorld[i] = glm::vec2(t1 * glm::vec4(localPoints[i], 0.f, 1.f));
        p_own_aabb_x.update(m_aabbPointsWorld[i].x);
        p_own_aabb_y.update(m_aabbPointsWorld[i].y);
    }

    // get the own projection


    for (const auto& tri : trimesh->getTri()) {
        // for each triangle, I get the projection of the AABB onto each transformed axis
        Projection p_aabb;
        bool collideWithCurrentTriangle = true;
        std::array<glm::vec2, 3> tri_points_w;
        for (size_t j = 0; j < 3; ++j) {
            tri_points_w[j] = glm::vec2(t2 * glm::vec4(tri.points[j], 0.f, 1.f));
        }

        for (size_t i = 0; i < 3; ++i) {
            glm::vec2 tri_axis_w = glm::vec2(t2 *glm::vec4(tri.axes[i], 0.f, 0.f));
            for (size_t j = 0; j < 4; ++j) {
                p_aabb.update(glm::dot(m_aabbPointsWorld[j], tri_axis_w));
            }
            Projection p_tri;
            // translate own projection by the correct amount
            for (size_t j = 0; j < 3; ++j) {
                p_tri.update(glm::dot(tri_points_w[j], tri_axis_w));
            }
            if (!p_aabb.intersect(p_tri)) {
                // found non-overlapping axis -> does not collide with this triangle
                collideWithCurrentTriangle = false;
            }
        }
        Projection p_tri_x, p_tri_y;
        for (size_t j = 0; j < 3; ++j) {
            p_tri_x.update(glm::dot(tri_points_w[j], glm::vec2(1.f, 0.f)));
            p_tri_y.update(glm::dot(tri_points_w[j], glm::vec2(0.f, 1.f)));
        }
        if (!p_own_aabb_x.intersect(p_tri_x) || !p_own_aabb_y.intersect(p_tri_y)) {
            collideWithCurrentTriangle = false;
        }
        if (collideWithCurrentTriangle) {
            return report;
        }

    }

    report.collide = false;
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
    if (s1 == nullptr || s2 == nullptr) {
        return CollisionReport();
    }
	auto it = m_functionMap.find(std::make_pair(s1->get_type_index(), s2->get_type_index()));
	if (it == m_functionMap.end()) {
        auto it2 = m_functionMap.find(std::make_pair(s2->get_type_index(), s1->get_type_index()));
        if (it2 == m_functionMap.end()) {
            return CollisionReport();
        }
        auto report = it2->second(s2, s1, t2, t1);
        if (report.collide) {
            report.direction *= -1.f;
        }
        return report;
	}

	return it->second(s1, s2, t1, t2);
}