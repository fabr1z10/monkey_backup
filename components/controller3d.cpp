#include "controller3d.h"
#include "../pyhelper.h"
#include "../node.h"
#include "../util.h"


Controller3D::Controller3D (const pybind11::kwargs& kwargs) : Controller(kwargs) {
	//m_maxClimbAngle = glm::radians(pyget<float>(kwargs, "max_climb_angle", 80.0f));
	//m_maxDescendAngle = glm::radians(pyget<float>(kwargs, "max_descend_angle", 80.0f));
	m_skinWidth = pyget<float>(kwargs, "skinWidth", .015f);
	//m_horizontalRayCount = pyget<int>(kwargs, "horizontal_ray_count", 4);
	//m_verticalRayCount = pyget<int>(kwargs, "vertical_ray_count", 4);
	m_maskDown = pyget<int>(kwargs, "mask_down", 2 | 32);
	m_maskUp = pyget<int>(kwargs, "mask_up", 2);
	m_halfSize = 0.5f * m_size;

}

void Controller3D::updateRaycastOrigins() {
	auto scale = m_node->getScale();
	glm::vec3 pos = m_node->getWorldPosition();

	pos += scale * m_center;
	auto scaledHalfSize = scale * m_halfSize;

	m_raycastOrigins.left = pos.x - scaledHalfSize.x;
	m_raycastOrigins.right = pos.x + scaledHalfSize.x;

	m_raycastOrigins.bottom = pos.y - scaledHalfSize.y;
	m_raycastOrigins.top = pos.y + scaledHalfSize.y;

	m_raycastOrigins.back = pos.z - scaledHalfSize.z;
	m_raycastOrigins.front = pos.z + scaledHalfSize.z;
}

void Controller3D::move(glm::vec3& delta, bool forced) {
	auto scale = m_node->getScale();
	if (delta != glm::vec3(0.0f)) {
		updateRaycastOrigins();
		m_details.reset();
		//m_ppp.clear();
		bool movingHorizontally = !isEqual(delta.x, 0.0f) || !isEqual(delta.z, 0.0f);
		if (movingHorizontally)
			horizontalCollisions(delta);
		if (!isEqual(delta.y, 0.0f))
			verticalCollisions(delta);
		glm::vec3 actualMove = delta / scale;
		m_node->move(actualMove);
//		if (!m_wasGnd && m_details.below) {
//			glm::vec3 p=m_entity->GetPosition();
//		}

	}
}

// this will have x and z components
void Controller3D::horizontalCollisions(glm::vec3& vel) {
	bool flipx = m_node->getFilpX();

	// TODO add back slope when everything is working fine
	if (fabs(vel.x) > 0.0f) {
		bool goingLeft = (flipx && vel.x > 0) || (!flipx && vel.x < 0);
		float directionX = goingLeft ? -1.0f : 1.0f;
		float rayLength = fabs(vel.x) + m_skinWidth;
		auto pos = m_node->getWorldPosition();
		float dx = goingLeft ? -m_halfSize[0] : m_halfSize[0];

		int i = 0;
		// try first moving along X direction
		float x = goingLeft ? m_raycastOrigins.left : m_raycastOrigins.right;
		std::array<glm::vec3, 2> raycastOrigins = {
				glm::vec3(x, m_raycastOrigins.bottom, m_raycastOrigins.front),
				glm::vec3(x, m_raycastOrigins.bottom, m_raycastOrigins.back)};

		for (const auto &r0 : raycastOrigins) {
			RayCastHit hit = m_collisionEngine->rayCast(r0, glm::vec3(directionX, 0.f, 0.f), rayLength, 2 | 32);
			if (hit.collide) {
				vel.x = std::max(hit.length - m_skinWidth, 0.0f) * sign(vel.x);
				rayLength = hit.length;
				m_details.left = directionX == -1.0f;
				m_details.right = directionX == 1.0f;
			}
		}
	}

	if (fabs(vel.z) > 0.0f) {
		float z = vel.z > 0 ? m_raycastOrigins.front : m_raycastOrigins.back;
		float directionZ = vel.z < 0 ? -1.0 : 1.0;
		glm::vec3 dirZ (0.0f, 0.0f, vel.z > 0 ? 1.0f : -1.0f);
		float dx = vel.x * (flipx ? -1.0f : 1.0f);
		std::array<glm::vec3, 2> raycastOrigins = {
				glm::vec3(m_raycastOrigins.left + dx, m_raycastOrigins.bottom, z),
				glm::vec3(m_raycastOrigins.right + dx, m_raycastOrigins.bottom, z)};
		float rayLength = fabs(vel.z) + m_skinWidth;
		for (const auto &r0 : raycastOrigins) {
			RayCastHit hit = m_collisionEngine->rayCast(r0, glm::vec3(0.0f, 0.0f, 1.0f) * directionZ, rayLength, 2 | 32);
			if (hit.collide) {
				vel.z = std::max(hit.length - m_skinWidth, 0.0f) * sign(vel.z);
				rayLength = hit.length;
				m_details.back = directionZ == -1;
				m_details.front = directionZ == 1;
			}
		}
	}
}

void Controller3D::verticalCollisions(glm::vec3& velocity) {

	float directionY = sign(velocity.y);
	float rayLength = std::abs(velocity.y) + m_skinWidth;
	Node* m_obstacle = nullptr;

	float velx = velocity.x * (m_node->getFilpX() ? -1.0f : 1.0f);
	float velz = velocity.z;
	glm::vec3 pos = m_node->getWorldPosition();
	std::array<glm::vec3, 4> raycastOrigins = {
			glm::vec3(m_raycastOrigins.left + velx, m_raycastOrigins.bottom, m_raycastOrigins.front + velz),
			glm::vec3(m_raycastOrigins.right + velx, m_raycastOrigins.bottom,m_raycastOrigins.front + velz),
			glm::vec3(m_raycastOrigins.left + velx, m_raycastOrigins.bottom, m_raycastOrigins.back + velz),
			glm::vec3(m_raycastOrigins.right + velx, m_raycastOrigins.bottom,m_raycastOrigins.back + velz)};

	for (const auto& r0 : raycastOrigins) {
		int collMask = (directionY == -1 ? (m_maskDown) : m_maskUp);
		RayCastHit hit = m_collisionEngine->rayCast(r0, glm::vec3(0.f, directionY, 0.f), rayLength, collMask);
		if (hit.collide) {
			velocity.y = (hit.length - m_skinWidth) * directionY;
			rayLength = hit.length;
			rayLength = hit.length;
			m_details.below = directionY == -1;
			m_details.above = directionY == 1;
			m_obstacle = hit.entity->getNode();
		}
	}






}
