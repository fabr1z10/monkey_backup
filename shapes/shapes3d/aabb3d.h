#pragma once


#include "../../shape.h"

class AABB3D : public Shape {
public:
	AABB3D(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);
//	bool isInside(glm::vec2 P) const override
private:
	glm::vec3 m_center;
	float m_xMin;
	float m_xMax;
	float m_yMin;
	float m_yMax;
	float m_zMin;
	float m_zMax;

};

