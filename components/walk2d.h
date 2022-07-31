#pragma once

#include "statemachine.h"
#include "controller.h"
#include "dynamics.h"
#include "renderer.h"

class Node;

class Walk2D : public State {
public:
	Walk2D(const std::string& id, const pybind11::kwargs&);
	//void init() override;
	void setParent(StateMachine*) override;
	void run(double) override;
    //void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;
    virtual void control() = 0;
protected:
	float m_gravity;
	float m_jumpHeight;
	float m_timeToJumpApex;
	float m_jumpVelocity;           // calculated
	float m_acceleration;
	float m_maxSpeed;
	float m_accelerationTime;
	Controller2D* m_controller;
	Dynamics* m_dynamics;
	Node* m_node;
	SpriteRenderer* m_spriteRenderer;
	std::string m_idleAnim;
    std::string m_walkAnim;
    std::string m_jumpAnim;
    bool m_left;
    bool m_right;
    bool m_up;
};


