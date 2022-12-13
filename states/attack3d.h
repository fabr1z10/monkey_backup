#pragma once

#include "../components/statemachine.h"
#include "../components/controller3d.h"
#include "../components/dynamics.h"
#include "../components/renderer.h"
#include "../components/keypad.h"

class Attack3D : public State {
public:
	Attack3D(const std::string& id, const pybind11::kwargs&);
	void init(const pybind11::kwargs& args) override;
	void run(double) override;

	void setParent(StateMachine*) override;
	//void keyCallback(int key) override;

private:
	// the animations to play
	std::vector<std::string> m_anims;
	std::string m_jumpAnim;
	Node* m_node;
	Controller3D* m_controller;
	Dynamics* m_dynamics;
	std::string m_exitState;
	float m_gravity;
	float m_acceleration;
	float m_maxSpeed;
	float m_accelerationTime;
	// index to the animation to play
	int m_currentAnim;
	AnimatedRenderer* m_renderer;
	KeyPad* m_keypad;
	float m_velocityThreshold;
	unsigned m_keyCombo;

};