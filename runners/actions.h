#include "scheduler.h"
#include "glm/glm.hpp"

class NodeAction : public Action {
public:
    explicit NodeAction(const pybind11::kwargs&);
    void start() override;
protected:
    Node* m_node;
    int m_nodeId;
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

// TODO Callfunc
//class CallFunc : public Action {
//
//};

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
    //void start() override;

private:
    int m_id;
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

};

class RemoveNode : public NodeAction {
public:
    explicit RemoveNode(const pybind11::kwargs& args) : NodeAction(args) {}
    int run(double) override;
};
