#include "aabb3d.h"

AABB3D::AABB3D(float xm, float xM, float ym, float yM, float zm, float zM) : Shape(), m_xMin(xm), m_xMax(xM), m_yMin(ym), m_yMax(yM) {
	m_bounds.min = glm::vec3(xm, ym, zm);
	m_bounds.max = glm::vec3(xM, yM, zM);
	m_center = 0.5f * (m_bounds.min + m_bounds.max);
	m_type = ShapeType::AABB3D;
}
