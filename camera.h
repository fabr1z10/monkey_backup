#pragma once

#include <glm/glm.hpp>
#include <array>
#include <pybind11/pybind11.h>
#include "shader.h"

namespace py = pybind11;

class Camera {
public:
	Camera(const py::kwargs& kwargs);
	virtual void setPosition(glm::vec3 eye, glm::vec3 direction, glm::vec3 up = glm::vec3(0, 1, 0));
	void setBounds(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);
	void init(Shader*);
	const glm::mat4& getViewMatrix() const;
protected:
	glm::vec3 m_fwd;
	glm::vec3 m_up;
	glm::vec3 m_eye;
	glm::vec2 m_xBounds;
	glm::vec2 m_yBounds;
	glm::vec2 m_zBounds;

	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;
	glm::vec4 m_viewport;
};

inline const glm::mat4 & Camera::getViewMatrix() const {
	return m_viewMatrix;
}


class OrthoCamera : public Camera {
public:
	OrthoCamera(float width, float height, const py::kwargs& kwargs);
private:
	float m_orthoWidth;
	float m_orthoHeight;

};


class PerspectiveCamera : public Camera {

};