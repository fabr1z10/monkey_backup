#pragma once

#pragma once

#include "statemachine.h"
#include "controller.h"
#include "dynamics.h"

class Node;

class Car2D : public State {
public:
	Car2D(const std::string& id, const pybind11::kwargs&);
	void init() override;
	void setParent(StateMachine*) override;
	void run(double) override;
private:
	float m_acceleration;
	float m_maxSpeed;
	float m_accelerationTime;
	float m_rotationSpeed;
	Controller* m_controller;
	Dynamics* m_dynamics;
	Node* m_node;
};


