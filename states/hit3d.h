#pragma once

#include "../components/statemachine.h"
#include "../components/controller3d.h"
#include "../components/dynamics.h"
#include "../components/renderer.h"
#include "../components/keypad.h"


class Node;

class Hit3D : public State {
public:
	Hit3D(const std::string& id, const pybind11::kwargs&);
	void init(const pybind11::kwargs& args) override;
	void run(double) override;
	void setParent(StateMachine*) override;

private:
	float m_speed;
	float m_verticalSpeed;
	int m_mode;
	std::string m_anim;
	float m_actualSpeed;
	Controller3D* m_controller;
	KeyPad* m_keypad;
	Dynamics* m_dynamics;
	float m_gravity;
	AnimatedRenderer* m_animatedRenderer;
	float m_timeToStop;
	float m_ax;
	std::string m_exitState;
};