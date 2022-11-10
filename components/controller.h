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
    virtual bool grounded() const = 0;
    void setSize(glm::vec3 size, glm::vec3 offset);
	virtual void move(glm::vec3&, bool forced);
	glm::vec3 getSize() const;
protected:
	void setDebugShape();
	Node* m_debugShape;
    void computeCoordinates();
	glm::vec3 m_size;
	glm::vec3 m_center;
	bool m_debug;
	CollisionEngine* m_collisionEngine;
    glm::vec3 m_localTopFwd;
    glm::vec3 m_localTopBack;
    glm::vec3 m_localBottomFwd;
    glm::vec3 m_localBottomBack;
};

inline glm::vec3 Controller::getSize() const {
    return m_size;
}

struct RaycastOrigins {
	glm::vec3 topFwd, topBack;
	glm::vec3 bottomFwd, bottomBack;
	//float xMin, xMax, yMin, yMax;
};

// this is for 2D platformers
class Controller2D : public Controller {
public:
	Controller2D(const pybind11::kwargs&);
	virtual ~Controller2D();
	void move(glm::vec3&, bool forced) override;
	std::type_index getType() override;
	void updateRaycastOrigins();
	bool grounded() const override;
	bool ceiling() const;
	bool left() const;
    bool right() const;
	void setGrounded(bool);
	void setPlatform(Platform*);
	void resetPlatform();
	bool isFalling(float);
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

inline bool Controller2D::ceiling () const {
    return m_details.above;
}

inline bool Controller2D::left () const {
    return m_details.left;
}
inline bool Controller2D::right () const {
    return m_details.right;
}


inline void Controller2D::setGrounded(bool value) {
    m_details.below = value;
}
