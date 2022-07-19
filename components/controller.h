#pragma once

#include "../component.h"
#include <glm/glm.hpp>
#include "../runners/collisionengine.h"
#include "platform.h"
/// a controller is a class that checks if node movements are valid

#include <pybind11/pybind11.h>


class Controller : public Component {
public:
	Controller(const pybind11::kwargs&);
	void start() override;

	virtual void move(glm::vec3&, bool forced);
protected:
	glm::vec3 m_size;
	glm::vec3 m_center;
	bool m_debug;
	CollisionEngine* m_collisionEngine;
};

struct RaycastOrigins {
	glm::vec3 topRight, topLeft;
	glm::vec3 bottomRight, bottomLeft;
	//float xMin, xMax, yMin, yMax;
};

// this is for 2D platformers
class Controller2D : public Controller {
public:
	Controller2D(const pybind11::kwargs&);
	void move(glm::vec3&, bool forced) override;
	std::type_index getType() override;
	void updateRaycastOrigins();
	bool grounded() const;
	void setGrounded(bool);
	void setPlatform(Platform*);
private:
	struct CollisionDetails {
		bool above, below;
		bool left, right;
		bool climbingSlope;
		bool descendingSlope;
		glm::vec3 velocityOld;
		float slopeAngle, slopeAngleOld;
		void reset();
	};

	void climbSlope(glm::vec3& velocity, float slopeAngle);
	void descendSlope(glm::vec3&);
	void horizontalCollisions(glm::vec3& velocity);
	void verticalCollisions(glm::vec3& velocity, bool);

	RaycastOrigins m_raycastOrigins;
	glm::vec3 m_localTopFwd;
	glm::vec3 m_localTopBack;
	glm::vec3 m_localBottomFwd;
	glm::vec3 m_localBottomBack;
	bool m_wasGnd;
	CollisionDetails m_details;
	float m_maxClimbAngle;
	float m_maxDescendAngle;
	float m_skinWidth;
	bool m_faceRight;
	int m_horizontalRayCount;
	int m_verticalRayCount;
	float m_horizontalRaySpacing;
	float m_verticalRaySpacing;
	int m_maskUp;
	int m_maskDown;
    // platforms on which I registered
    Platform* m_platforms;
};

inline bool Controller2D::grounded() const {
    return m_details.below;
}

inline void Controller2D::setGrounded(bool value) {
    m_details.below = value;
}
