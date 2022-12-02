#pragma once

#include "controller.h"

struct RaycastOrigins3D {
	float left, right;
	float bottom, top;
	float front, back;
};

struct CollisionDetails3D {
	bool above, below;
	bool left, right;
	bool front, back;

	bool climbingSlope;
	bool descendingSlope;

	float slopeAngle, slopeAngleOld;
	void reset();
};

class Controller3D : public Controller {
public:
	Controller3D(const pybind11::kwargs&);
	void move(glm::vec3&, bool forced) override;

private:
	float m_skinWidth;
	// TODO replace later
	//float m_maxClimbAngle;
	//float m_maxDescendAngle;
	int m_maskUp;
	int m_maskDown;
	glm::vec3 m_halfSize;

	RaycastOrigins3D m_raycastOrigins;
	CollisionDetails3D m_details;

	void updateRaycastOrigins();
	void horizontalCollisions(glm::vec3& velocity);
	void verticalCollisions(glm::vec3& velocity);
};
