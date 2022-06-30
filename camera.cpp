#include "camera.h"
#include "engine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <pybind11/stl.h>
#include <cassert>

// ** every ** camera needs to have a viewport. A viewport represents
// a portion of the device size where the camera projects its image.
// viewport is 4 numbers: first two is the bottom left position of the rectangle,
// and the last 2 are the size (width and height)
Camera::Camera(const py::kwargs& kwargs) {
	auto deviceSize = Engine::instance().getDeviceSize();
	m_viewport = glm::vec4(0, 0, deviceSize[0], deviceSize[1]);
	if (kwargs) {
		if (kwargs.contains("viewport")) {
			auto viewport = kwargs["viewport"].cast<std::vector<int>>();
			m_viewport = glm::vec4(viewport[0], viewport[1], viewport[2], viewport[3]);
		}
	}
	assert(m_viewport.x >= 0 && m_viewport.x <= deviceSize[0]);
	assert(m_viewport.y >= 0 && m_viewport.y <= deviceSize[1]);
	assert(m_viewport.z >= 0 && m_viewport.z <= deviceSize[0]);
	assert(m_viewport.w >= 0 && m_viewport.y <= deviceSize[1]);
	setPosition(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));


}

OrthoCamera::OrthoCamera(float width, float height, const py::kwargs& kwargs) : Camera(kwargs), m_orthoWidth(width), m_orthoHeight(height) {
	float hw = m_orthoWidth / 2.0f;
	float hh = m_orthoHeight / 2.0f;
	m_projectionMatrix = glm::ortho(-hw, hw, -hh, hh, -100.0f, 100.0f);

}

void Camera::setPosition(glm::vec3 eye, glm::vec3 dir, glm::vec3 up) {
	m_fwd = dir;
	m_up = up;
	m_eye = eye;
	m_viewMatrix = glm::lookAt(eye, eye + dir, up);
}

void Camera::init(Shader* s) {
	// setup projection and viewport
	s->setMat4("projection", m_projectionMatrix);
	//glUniformMatrix4fv(glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, &m_projectionMatrix[0][0]);
	auto vp = Engine::instance().getActualDeviceViewport();

	float x = vp.x + m_viewport.x * vp[2];
	float y = vp.y + m_viewport.y * vp[3];
	float w = m_viewport[2] * vp[2];
	float h = m_viewport[3] * vp[3];
	glViewport(x, y, w, h);


}

void Camera::setBounds(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax) {}