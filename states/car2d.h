#pragma once

#pragma once

#include "../components/statemachine.h"
#include "../components/controller.h"
#include "../components/dynamics.h"

class Node;

class Car2D : public State {
public:
	Car2D(const std::string& id, const pybind11::kwargs&);
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


