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

class SetState : public NodeAction {
public:
    explicit SetState(const pybind11::kwargs&);
    int run(double) override;
    //void start() override;
private:
    std::string m_state;

};