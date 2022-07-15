#include "controller.h"
#include "../node.h"
#include <glm/gtx/matrix_decompose.hpp>
#include "../util.h"
#include "../pyhelper.h"
#include "../engine.h"
#include <glm/gtx/transform.hpp>

void Controller::start() {
	auto& engine = Engine::instance();
	auto room = engine.getRoom();
	m_collisionEngine = room->getRunner<CollisionEngine>();
}

Controller::Controller(const pybind11::kwargs& args) {
    m_size = dictget<glm::vec3>(args, "size", glm::vec3(0.f));
    m_center = dictget<glm::vec3>(args, "center", glm::vec3(0.f));
}

Controller2D::Controller2D(const pybind11::kwargs& kwargs) : Controller(kwargs) {
	m_maxClimbAngle = glm::radians(pyget<float>(kwargs, "max_climb_angle", 80.0f));
	m_maxDescendAngle = glm::radians(pyget<float>(kwargs, "max_descend_angle", 80.0f));
	m_skinWidth = pyget<float>(kwargs, "skinWidth", .015f);
	m_horizontalRayCount = pyget<int>(kwargs, "horizontal_ray_count", 4);
	m_verticalRayCount = pyget<int>(kwargs, "vertical_ray_count", 4);
	m_maskDown = pyget<int>(kwargs, "mask_down", 2 | 32);
	m_maskUp = pyget<int>(kwargs, "mask_up", 2);


	// raycast origins in local coordinates. This won't change until shape is changed
	m_localTopFwd = m_size - m_center;
	m_localTopBack = glm::vec3(0.0f, m_size.y, 0.0f) - m_center;
	m_localBottomFwd = glm::vec3(m_size.x, 0.0f, 0.0f) - m_center;
	m_localBottomBack = glm::vec3(0.0f) - m_center;
}

void Controller2D::updateRaycastOrigins() {
	auto worldMatrix = m_node->getWorldMatrix();
	m_raycastOrigins.topForward = worldMatrix * glm::vec4(m_localTopFwd, 1.0f);
	m_raycastOrigins.topBack = worldMatrix * glm::vec4(m_localTopBack, 1.0f);
	m_raycastOrigins.bottomForward = worldMatrix * glm::vec4(m_localBottomFwd, 1.0f);
	m_raycastOrigins.bottomBack = worldMatrix * glm::vec4(m_localBottomBack, 1.0f);
	m_faceRight = m_raycastOrigins.topForward.x > m_raycastOrigins.topBack.x;
	if (m_raycastOrigins.topForward.x > m_raycastOrigins.topBack.x) {
		m_raycastOrigins.xMin = m_raycastOrigins.topBack.x;
		m_raycastOrigins.xMax = m_raycastOrigins.topForward.x;
	} else {
		m_raycastOrigins.xMax = m_raycastOrigins.topBack.x;
		m_raycastOrigins.xMin = m_raycastOrigins.topForward.x;
	}
	float width = fabs(m_raycastOrigins.topForward.x - m_raycastOrigins.topBack.x);
	float height = m_raycastOrigins.topForward.y - m_raycastOrigins.bottomForward.y;
	m_horizontalRaySpacing = width / (m_horizontalRayCount - 1);
	m_verticalRaySpacing = height / (m_verticalRayCount - 1);
}

void Controller::move(glm::vec3 & delta) {
    m_node->move(glm::translate(delta));
}

void Controller2D::move(glm::vec3& delta) {

	if (delta == glm::vec3(0.0f)) return;

	updateRaycastOrigins();

	// check if character was grounded at last iteration
	m_wasGnd = m_details.below;
	m_details.reset();

	if (delta.y < 0 && m_wasGnd) {
        descendSlope(delta);
    }
	if (!isEqual(delta.x, 0.0f))
		horizontalCollisions(delta);
	if (!isEqual(delta.y, 0.0f))
		verticalCollisions(delta);
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
	float directionX = !(goingForward != m_faceRight);
	auto r0 = goingForward ? m_raycastOrigins.bottomBack : m_raycastOrigins.bottomForward;
	RayCastHit hit = m_collisionEngine->rayCast(r0, glm::vec3(0.f, 0.f, -1.f), 100.0f, m_maskDown);
	if (hit.collide) {
		float slopeAngle = angle(hit.normal, glm::vec3(0.f, 1.f, 0.f));
		if (slopeAngle != 0 && slopeAngle <= m_maxDescendAngle) {
			if (sign(hit.normal.x) == directionX) {
				if (hit.length - m_skinWidth <= tan(slopeAngle) * fabs(velocity.x)) {
					float moveDistance = fabs(velocity.x);
					float descendVelocityY = sin(slopeAngle) * moveDistance;
					velocity.x = cos(slopeAngle) * moveDistance;// * sign(velocity.x);
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
	bool goingForward = velocity.x > 0.0f;
	float directionX = !(goingForward != m_faceRight);
	float rayLength = fabs(velocity.x) + m_skinWidth;
	float sgnx = sign(velocity.x);
	auto r0 = goingForward ? m_raycastOrigins.bottomForward : m_raycastOrigins.bottomBack;
	//std::cout << "r0 = " << r0.x << ", " << r0.y << "\n";
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
				//std::cout << "angle = " << slopeAngle * Rad2Deg << "\n";
				float distanceToSlopeStart = 0;
				if (slopeAngle != m_details.slopeAngleOld) {
					distanceToSlopeStart = hit.length - m_skinWidth;
					velocity.x -= distanceToSlopeStart * directionX;
				}
				climbSlope(velocity, slopeAngle);
				velocity.x += distanceToSlopeStart * directionX;

			}

			if (!m_details.climbingSlope || slopeAngle > m_maxClimbAngle) {
				velocity.x = std::max(hit.length - m_skinWidth, 0.0f) * sgnx;
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

void Controller2D::verticalCollisions(glm::vec3& velocity) {
	float directionY = sign(velocity.y);
	bool goingForward = velocity.x > 0.0f;
	float directionX = !(goingForward != m_faceRight);
	float rayLength = std::abs(velocity.y) + m_skinWidth;
	float obstacleDistance = std::numeric_limits<float>::max();
	std::unordered_set<Node*> obstacles;

	float speedX = fabs(velocity.x);

	glm::vec3 r0(m_raycastOrigins.xMin, 0.f, 0.f);
	r0.y = directionY > 0 ? m_raycastOrigins.topForward.y : m_raycastOrigins.bottomForward.y;
	for (int i = 0; i < m_verticalRayCount; i++) {
		auto rayOrigin = r0 + glm::vec3(directionX, 0.f, 0.f) * (speedX + i * m_verticalRaySpacing);
		int collMask = (directionY == -1 ? (m_maskDown) : m_maskUp);
		RayCastHit hit = m_collisionEngine->rayCast(rayOrigin, glm::vec3(0.f, directionY, 0.f), rayLength, collMask);
		if (hit.collide) {
			velocity.y = (hit.length - m_skinWidth) * directionY;
			rayLength = hit.length;
			if (m_details.climbingSlope) {
				velocity.x = (velocity.y / tan(m_details.slopeAngle)) * sign(velocity.x);
			}
			m_details.below = directionY == -1;
			m_details.above = directionY == 1;
			if (directionY < 0.0f) {
				if (hit.length < obstacleDistance) {
					obstacles.clear();
					obstacleDistance = hit.length;
				}
				obstacles.insert(hit.entity->getNode());
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
//	// register to new platforms
//	for (const auto& o : obstacles) {
//		auto platformController = o->GetComponent<PlatformComponent>();
//		if (platformController != nullptr) {
//			platformController->Register(this);
//			m_platforms.push_back(o);
//		}
//
//	}

}


std::type_index Controller2D::getType() {
	return std::type_index(typeid(Controller));
}


void Controller2D::climbSlope(glm::vec3& velocity, float slopeAngle) {
	float moveDistance = fabs(velocity.x);
	float climbVelocityY = sin(slopeAngle) * moveDistance;
	if (velocity.y <= climbVelocityY) {
		velocity.y = climbVelocityY;
		velocity.x = cos(slopeAngle) * moveDistance * sign(velocity.x);
		m_details.below = true;
		m_details.climbingSlope = true;
		m_details.slopeAngle = slopeAngle;
	}
}

