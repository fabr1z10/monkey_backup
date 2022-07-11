#pragma once

#include "statemachine.h"
#include "controller.h"
#include "dynamics.h"

class Node;

class Walk2D : public State {
public:
	Walk2D(const std::string& id, const pybind11::kwargs&);
	void init() override;
	void setParent(StateMachine*) override;
	void run(double) override;
private:
	float m_gravity;
	float m_acceleration;
	float m_maxSpeed;
	float m_accelerationTime;
	Controller* m_controller;
	Dynamics* m_dynamics;
	Node* m_node;
};


