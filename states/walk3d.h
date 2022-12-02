#pragma once

#include "../components/statemachine.h"
#include "../components/controller3d.h"
#include "../components/dynamics.h"
#include "../components/renderer.h"

class Node;

class Walk3D : public State {
public:
	Walk3D(const std::string& id, const pybind11::kwargs&);
	void init(const pybind11::kwargs& args) override;
	void setParent(StateMachine*) override;
	void run(double) override;
	virtual void control() = 0;
protected:
	float m_gravity;
	float m_jumpHeight;
	float m_timeToJumpApex;
	float m_jumpVelocity;           // calculated
	float m_acceleration;
	float m_maxSpeedGround;
	float m_maxSpeedAir;
	float m_accelerationTime;
	float m_velocityThreshold;
	Controller3D* m_controller;
	Dynamics* m_dynamics;
	Node* m_node;
	AnimatedRenderer* m_animatedRenderer;
	std::string m_idleAnim;
	std::string m_walkAnim;
	std::string m_jumpAnim;
	unsigned m_keys;
	bool m_left;
	bool m_right;
	bool m_up;
	bool m_down;
	bool m_jmp;
};

