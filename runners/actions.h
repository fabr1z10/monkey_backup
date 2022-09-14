#pragma once

#include "scheduler.h"
#include "glm/glm.hpp"

class Renderer;

class NodeAction : public Action {
public:
    explicit NodeAction(const pybind11::kwargs&);
    void start() override;
protected:
    Node* m_node;
    int m_nodeId;
};

class Animate : public NodeAction {
public:
	Animate(const pybind11::kwargs&);
	void start() override;
	int run(double) override;
private:
	std::string m_animation;
};

class Delay : public Action {
public:
    Delay(float t);
    int run(double) override;
    void start() override;
private:
    float m_time;
    float m_timer;
};

class Blink : public NodeAction {
public:
    Blink(const pybind11::kwargs&);
    int run(double) override;
    void start() override;
    void end() override;
private:
    float m_duration;
    float m_period;
    float m_time;
    Renderer* m_renderer;

};



class CallFunc : public Action {
public:
    explicit CallFunc(pybind11::function f);
    int run(double) override;
    void start() override {}
private:
    pybind11::function m_func;
};

class Repeat : public Action {
public:
	explicit Repeat(pybind11::function f, float);
	int run(double) override;
	void start() override {}
private:
	float m_timer;
	float m_period;
	pybind11::function m_func;
};

class MoveBy : public NodeAction {
public:
    MoveBy(const pybind11::kwargs&);
    int run(double) override;
    void start() override;

private:
    int m_id;
    glm::vec3 m_endPoint;
    float m_speed;
    float m_distance;
    float m_distanceTraveled;
    glm::vec3 m_delta;
    glm::vec3 m_unitVec;
};

class MoveAccelerated : public NodeAction {
public:
    explicit MoveAccelerated(const pybind11::kwargs&);
    int run(double) override;
    void start() override;

private:
    int m_id;
    float m_time;
    float m_timeOut;
    glm::vec3 m_endPoint;
    glm::vec3 m_initialVelocity;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
};


class SetState : public NodeAction {
public:
    explicit SetState(const pybind11::kwargs&);
    int run(double) override;
    //void start() override;
private:
    std::string m_state;
    pybind11::kwargs m_args;
};

class RemoveNode : public NodeAction {
public:
    explicit RemoveNode(const pybind11::kwargs& args) : NodeAction(args) {}
    int run(double) override;
};
