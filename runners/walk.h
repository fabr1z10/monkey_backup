#pragma once


#include "actions.h"



class Walk : public NodeAction {
public:
    Walk(const pybind11::kwargs&);
    int run(double) override;
    void start() override;

private:
    struct PathSegment {
        glm::vec2 start;
        glm::vec2 end;
        glm::vec2 direction;
        float length;
        std::string animation;
        bool flip;

    };
    glm::vec2 m_targetPos;
    std::vector<PathSegment> m_segments;
    float m_speed;
    int m_currentSegment;
    float m_currentLength;


};