#include "controller.h"
#include "../node.h"
#include <glm/gtx/matrix_decompose.hpp>
#include "../util.h"
#include "../pyhelper.h"
#include "../engine.h"
#include "platform.h"
#include <glm/gtx/transform.hpp>

void Controller::start() {
	auto& engine = Engine::instance();
	auto room = engine.getRoom();

	m_collisionEngine = room->getRunner<CollisionEngine>();
}

void Controller::setSize(pybind11::tuple size, pybind11::tuple center) {
	m_size = glm::vec3(size[0].cast<float>(), size[1].cast<float>(), size.size() > 2 ? size[2].cast<float>() : 0.f);
	m_center = glm::vec3(center[0].cast<float>(), center[1].cast<float>(), center.size() > 2 ? center[2].cast<float>() : 0.f);
	computeCoordinates();
}

Controller::Controller(const pybind11::kwargs& args) {
    m_size = dictget<glm::vec3>(args, "size", glm::vec3(0.f));
    m_center = dictget<glm::vec3>(args, "center", glm::vec3(0.f));
	computeCoordinates();
}

void Controller::computeCoordinates() {
	// raycast origins in local coordinates. This won't change until shape is changed
	m_localTopFwd = m_size - m_center;
	m_localTopBack = glm::vec3(0.0f, m_size.y, 0.0f) - m_center;
	m_localBottomFwd = glm::vec3(m_size.x, 0.0f, 0.0f) - m_center;
	m_localBottomBack = glm::vec3(0.0f) - m_center;
}

Controller2D::Controller2D(const pybind11::kwargs& kwargs) : Controller(kwargs) {
	m_maxClimbAngle = glm::radians(pyget<float>(kwargs, "max_climb_angle", 80.0f));
	m_maxDescendAngle = glm::radians(pyget<float>(kwargs, "max_descend_angle", 80.0f));
	m_skinWidth = pyget<float>(kwargs, "skinWidth", .015f);
	m_horizontalRayCount = pyget<int>(kwargs, "horizontal_ray_count", 4);
	m_verticalRayCount = pyget<int>(kwargs, "vertical_ray_count", 4);
	m_maskDown = pyget<int>(kwargs, "mask_down", 2 | 32);
	m_maskUp = pyget<int>(kwargs, "mask_up", 2);
    m_platforms = nullptr;


}

Controller2D::~Controller2D() {
    if (m_platforms != nullptr) {
        m_platforms->unregisterComponent(this);
        m_platforms->forceRemove(this);

    }
}

void Controller2D::updateRaycastOrigins() {
	auto worldMatrix = m_node->getWorldMatrix();
	m_raycastOrigins.topRight = worldMatrix * glm::vec4(m_localTopFwd, 1.0f);
	m_raycastOrigins.topLeft = worldMatrix * glm::vec4(m_localTopBack, 1.0f);
	m_raycastOrigins.bottomRight = worldMatrix * glm::vec4(m_localBottomFwd, 1.0f);
	m_raycastOrigins.bottomLeft = worldMatrix * glm::vec4(m_localBottomBack, 1.0f);
	float width = fabs(m_raycastOrigins.topRight.x - m_raycastOrigins.topLeft.x);
	float height = m_raycastOrigins.topRight.y - m_raycastOrigins.bottomRight.y;
	m_horizontalRaySpacing = height / (m_horizontalRayCount - 1.f);
	m_verticalRaySpacing = width / (m_verticalRayCount - 1.f);
}

void Controller::move(glm::vec3 & delta, bool) {
    m_node->move(glm::translate(delta));
}

void Controller2D::move(glm::vec3& delta, bool forced) {

	if (delta == glm::vec3(0.0f)) return;

	updateRaycastOrigins();

	// check if character was grounded at last iteration
	m_wasGnd = m_details.below;
	m_details.reset();
	if (forced) {
	    m_details.below = true;
	}

	if (delta.y < 0 && m_wasGnd) {
        descendSlope(delta);
    }
	if (!isEqual(delta.x, 0.0f))
		horizontalCollisions(delta);
	if (!isEqual(delta.y, 0.0f))
		verticalCollisions(delta, forced);
    m_node->move(glm::translate(delta));


}


void Controller2D::CollisionDetails::reset() {
    above = below = false;
    left = right = false;
    climbingSlope = false;
    descendingSlope = false;
    slopeAngleOld = slopeAngle;
    slopeAngle = 0.0f;
}

void Controller2D::descendSlope(glm::vec3& velocity) {
	if (velocity.x == 0.0f) return;
	bool goingForward = velocity.x > 0.0f;
	auto directionX = signf(velocity.x);
	auto r0 = directionX > 0.f ? m_raycastOrigins.bottomLeft : m_raycastOrigins.bottomRight;
	RayCastHit hit = m_collisionEngine->rayCast(r0, glm::vec3(0.f, -1.0f, 0.f), 100.0f, m_maskDown);
	if (hit.collide) {
		float slopeAngle = angle(hit.normal, glm::vec3(0.f, 1.f, 0.f));
		if (slopeAngle != 0 && slopeAngle <= m_maxDescendAngle) {
			if (signf(hit.normal.x) == directionX) {
				if (hit.length - m_skinWidth <= tan(slopeAngle) * fabs(velocity.x)) {
					float moveDistance = fabs(velocity.x);
					float descendVelocityY = sin(slopeAngle) * moveDistance;
					velocity.x = directionX *cos(slopeAngle) * moveDistance;// * sign(velocity.x);
					velocity.y -= descendVelocityY;
					m_details.slopeAngle = slopeAngle;
					m_details.descendingSlope = true;
					m_details.below = true;
				}
			}
		}
	}
}

void Controller2D::horizontalCollisions(glm::vec3& velocity) {
    float directionX = signf(velocity.x);
	float rayLength = fabs(velocity.x) + m_skinWidth;
	auto r0 = directionX > 0.f ? m_raycastOrigins.bottomRight : m_raycastOrigins.bottomLeft;
	for (int i = 0; i < m_horizontalRayCount; i++) {
		auto rayOrigin = r0 + glm::vec3(0.0f, i * m_horizontalRaySpacing, 0.0f);
		RayCastHit hit = m_collisionEngine->rayCast(rayOrigin, glm::vec3(directionX, 0.f, 0.f), rayLength, 2 | 32);
		if (hit.collide) {
			float slopeAngle = angle(hit.normal, glm::vec3(0.f, 1.f, 0.f));
			if (i == 0 && slopeAngle <= m_maxClimbAngle) {
				if (m_details.descendingSlope) {
					m_details.descendingSlope = false;
					velocity = m_details.velocityOld;
				}
				float distanceToSlopeStart = 0;
				if (slopeAngle != m_details.slopeAngleOld) {
					distanceToSlopeStart = hit.length - m_skinWidth;
					velocity.x -= distanceToSlopeStart * directionX;
				}
				climbSlope(velocity, slopeAngle);
				velocity.x += distanceToSlopeStart * directionX;
			}
			if (!m_details.climbingSlope || slopeAngle > m_maxClimbAngle) {
				velocity.x = std::max(hit.length - m_skinWidth, 0.0f) * directionX;
				rayLength = hit.length;
				if (m_details.climbingSlope) {
					velocity.y = tan(m_details.slopeAngle)* fabs(velocity.x);
				}
				m_details.left = directionX == -1;
				m_details.right = directionX == 1;
			}
		}
	}
}

void Controller2D::verticalCollisions(glm::vec3& velocity, bool forced) {
	auto directionY = signf(velocity.y);
	bool goingForward = velocity.x > 0.0f;
	float directionX = (goingForward == m_faceRight) ? 1.f : -1.f;
	float rayLength = std::abs(velocity.y) + m_skinWidth;
	float obstacleDistance = std::numeric_limits<float>::max();
	//std::unordered_set<Node*> obstacles;
	Node* obstacle = nullptr;
	float speedX = fabs(velocity.x);
    bool atleast = false;
	//glm::vec3 r0(m_raycastOrigins.xMin, 0.f, 0.f);
	glm::vec3 r0 = directionY > 0.f ? m_raycastOrigins.topLeft : m_raycastOrigins.bottomLeft;
	for (int i = 0; i < m_verticalRayCount; i++) {
		auto rayOrigin = r0 + glm::vec3(i * m_verticalRaySpacing + velocity.x, 0.f, 0.f) ;
		int collMask = (directionY == -1 ? (m_maskDown) : m_maskUp);
		RayCastHit hit = m_collisionEngine->rayCast(rayOrigin, glm::vec3(0.f, directionY, 0.f), rayLength, collMask);
		if (hit.collide) {
		    atleast = true;
			velocity.y = (hit.length - m_skinWidth) * directionY;
			rayLength = hit.length;
			if (m_details.climbingSlope) {
				velocity.x = (velocity.y / tan(m_details.slopeAngle)) * signf(velocity.x);
			}
			m_details.below = directionY == -1;
			m_details.above = directionY == 1;
			if (directionY < 0.0f) {
				if (hit.length < obstacleDistance) {
				    obstacle = hit.entity->getNode();
					obstacleDistance = hit.length;
				}
				//obstacles.insert(hit.entity->getNode());
			}
		}
	}


	// TODO for tomorrow
	// unregister to ALL platforms I am not on top anymore
//	if (!m_platforms.empty()) {
//		auto j = m_platforms.begin();
//		while (j != m_platforms.end()) {
//			auto i = obstacles.begin();
//			bool found = false;
//			while (i != obstacles.end())
//			{
//				if ((*j)->GetId() == (*i)->GetId()) {
//					// ok, this means o is already registered
//					obstacles.erase(i++);
//					found = true;
//					break;
//				} else {
//					i++;
//				}
//			}
//			if (!found) {
//				// unregister
//				auto platformController = (*j)->GetComponent<PlatformComponent>();
//				platformController->Unregister(this);
//				m_platforms.erase(j++);
//			} else {
//				j++;
//			}
//		}
//	}
//
	// register to new platforms -- needed for moving platforms+
	if (obstacle != nullptr) {
		auto* platformController = obstacle->getComponent<Platform>();

		if (platformController != m_platforms) {
		    if (platformController != nullptr) {
                platformController->registerComponent(this);
            }
		    if (m_platforms != nullptr) {
		        m_platforms->unregisterComponent(this);
		    }
			m_platforms = platformController;
		}
	} else {
	    if (m_platforms != nullptr && !forced) {
            m_platforms->unregisterComponent(this);
            m_platforms = nullptr;
	    }
	}

}

void Controller2D::setPlatform(Platform * p) {
    m_details.below = true;
    if (m_platforms != nullptr) {
        m_platforms->unregisterComponent(this);
    }
    m_platforms = p;
}

void Controller2D::resetPlatform() {
    m_platforms = nullptr;
}

std::type_index Controller2D::getType() {
	return std::type_index(typeid(Controller));
}


void Controller2D::climbSlope(glm::vec3& velocity, float slopeAngle) {
	float moveDistance = fabs(velocity.x);
	float climbVelocityY = sin(slopeAngle) * moveDistance;
	if (velocity.y <= climbVelocityY) {
		velocity.y = climbVelocityY;
		velocity.x = cos(slopeAngle) * moveDistance * signf(velocity.x);
		m_details.below = true;
		m_details.climbingSlope = true;
		m_details.slopeAngle = slopeAngle;
	}
}

bool Controller2D::isFalling(float dir) {
    updateRaycastOrigins();
    auto rayOrigin = (dir < 0.f ? m_raycastOrigins.bottomLeft : m_raycastOrigins.bottomRight);

    //glm::vec2 rayOrigin = (dir == -1) ? m_raycastOrigins.bottomLeft : m_raycastOrigins.bottomRight;
    RayCastHit hit = m_collisionEngine->rayCast(rayOrigin, glm::vec3(0.f, -1.f, 0.f), 5.0, 2|32);
    if (!hit.collide)
        return true;
    return false;
}