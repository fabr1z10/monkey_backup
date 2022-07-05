#pragma once

#include "bounds.h"

enum class ShapeType {
	SEGMENT, RECT, CIRCLE, PLANE, SPHERE, AABB, CONVEXPOLY, COMPOUND, POLY, POLYLINE, PRISM, PSEUDO3D
};


class Shape {
public:
	Shape() = default;

	glm::vec3 getOffset() const;
	void setOffset(float x, float y, float z);

	Bounds getBounds() const;
	ShapeType getShapeType() const;
protected:
	Bounds m_bounds;
	glm::vec3 m_offset;
	ShapeType m_type;
};

inline glm::vec3 Shape::getOffset() const {
	return m_offset;
}

inline void Shape::setOffset(float x, float y, float z) {
	m_offset = glm::vec3(x, y, z);
}

inline ShapeType Shape::getShapeType() const {
	return m_type;
}

inline Bounds Shape::getBounds() const {
	return m_bounds;
}


class Shape2D : public Shape {
public:
	// project shape onto axis
	virtual glm::vec2 project(glm::vec2, const glm::mat4&) const = 0;
};


