#include "camera.h"
#include "engine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <pybind11/stl.h>
#include <cassert>
#include "pyhelper.h"

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
//	assert(m_viewport.x >= 0 && m_viewport.x <= deviceSize[0]);
//	assert(m_viewport.y >= 0 && m_viewport.y <= deviceSize[1]);
//	assert(m_viewport.z >= 0 && m_viewport.z <= deviceSize[0]);
//	assert(m_viewport.w >= 0 && m_viewport.y <= deviceSize[1]);
	auto t = std::numeric_limits<float>::infinity();
	m_xBounds = glm::vec2(-t, t);
	m_yBounds = m_xBounds;
    m_zBounds = m_xBounds;
	setPosition(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));


}

OrthoCamera::OrthoCamera(float width, float height, const py::kwargs& kwargs) : Camera(kwargs), m_orthoWidth(width), m_orthoHeight(height) {
	float hw = m_orthoWidth / 2.0f;
	float hh = m_orthoHeight / 2.0f;
	m_projectionMatrix = glm::ortho(-hw, hw, -hh, hh, -100.0f, 100.0f);

}

glm::vec2 OrthoCamera::getWorldCooridnates(float x, float y) {
    float x0 = -m_viewMatrix[3][0] - m_orthoWidth * 0.5f;
    float y0 = -m_viewMatrix[3][1] - m_orthoHeight * 0.5f;
    float winHeight = Engine::instance().getWindowSize().y;
    float ty = winHeight - y;
    float xw = x0 + (x - m_screenViewport.x) * (m_orthoWidth / m_screenViewport[2]);
    float yw = y0 + (ty - m_screenViewport.y) * (m_orthoHeight / m_screenViewport[3]);
    return glm::vec2(xw, yw);
}

PerspectiveCamera::PerspectiveCamera(const py::kwargs& kwargs) : Camera(kwargs) {
	m_fov = glm::radians(dictget(kwargs, "fov", 45.0f));
	m_near = dictget(kwargs, "near", 0.05f);
	m_far = dictget(kwargs, "far", 100.0f);
	m_projectionMatrix = glm::perspective (m_fov, m_viewport[2]/m_viewport[3], m_near, m_far);

}


void Camera::setPosition(glm::vec3 eye, glm::vec3 dir, glm::vec3 up) {
	m_fwd = dir;
	m_up = up;
	m_eye = eye;
	m_eye.x = std::clamp(m_eye.x, m_xBounds[0], m_xBounds[1]);
    m_eye.y = std::clamp(m_eye.y, m_yBounds[0], m_yBounds[1]);
    m_eye.z = std::clamp(m_eye.z, m_zBounds[0], m_zBounds[1]);
	m_viewMatrix = glm::lookAt(m_eye, m_eye + dir, up);
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
	m_screenViewport = glm::vec4(x, y, x+w, y+h);
	glViewport(x, y, w, h);


}

void Camera::setBounds(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax) {
    m_xBounds = glm::vec2(xMin, xMax);
    m_yBounds = glm::vec2(yMin, yMax);
    m_zBounds = glm::vec2(zMin, zMax);
}

bool Camera::isInViewport(float x, float y) {
    float winHeight = Engine::instance().getWindowSize().y;
    float yf = winHeight - y;
    if (x < m_screenViewport[0] || x > m_screenViewport[2]) {
        return false;
    }
    if (yf < m_screenViewport[1] || yf > m_screenViewport[3]) {
        return false;
    }
    return true;

}