#include "scheduler.h"
#include "glm/glm.hpp"

class MoveBy : public Action {
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