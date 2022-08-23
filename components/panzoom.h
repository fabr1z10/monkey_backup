#pragma once


#include "../component.h"
#include "../mouselistener.h"
#include "hotspot.h"

class OrthoCamera;

class PanZoom : public Component, public MouseListener {
public:
	PanZoom() = default;
	//virtual ~PanZoom();
	void cursorPosCallback(GLFWwindow*, double, double) override;
	void mouseButtonCallback(GLFWwindow*, int, int, int) override;

	void start() override;
private:
	OrthoCamera* m_camera;
	double m_lastX;
	double m_lastY;
};
