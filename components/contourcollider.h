#pragma once

#include "collider.h"
#include <vector>

class ContourCollider : public Collider {
public:
    ContourCollider(const std::string& imageFile, float eps, int flag, int mask, int tag, glm::vec2 anchorPoint = glm::vec2(0.f));
    std::shared_ptr<Shape> getShape() override;
    int getCollisionTag() const override;
    int getCollisionFlag() const override;
    int getCollisionMask() const override;
    void setCollisionFlag(int) override;
private:
    void generateDebugMesh() override;

    enum class StepDirection
    {
        NONE,
        UP,
        LEFT,
        DOWN,
        RIGHT
    };
    bool isPixelSolid(int x, int y);
    glm::ivec2 findStartPoint();
    void step(int x, int y);

    std::vector<float> walkPerimeter(int startX, int startY);
    //void generateDebugMesh() override;
    std::shared_ptr<Shape> m_shape;
    int m_flag;
    int m_mask;
    int m_tag;
    int m_width;
    int m_height;
    int m_size;
    std::vector<unsigned char> m_data;
    std::vector<float> m_contour;
    std::vector<glm::vec2> mm;
    // The direction we previously stepped
    StepDirection previousStep;

    // Our next step direction:
    StepDirection nextStep;
};

inline std::shared_ptr<Shape> ContourCollider::getShape() {
    return m_shape;
}

inline int ContourCollider::getCollisionTag() const {
    return m_tag;
}

inline int ContourCollider::getCollisionFlag() const {
    return m_flag;
}

inline int ContourCollider::getCollisionMask() const {
    return m_mask;
}

inline void ContourCollider::setCollisionFlag(int flag) {
    m_flag = flag;
}

