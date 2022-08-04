#pragma once

#include "collider.h"
#include "renderer.h"

class SpriteCollider : public Collider {
public:
    SpriteCollider(int flag, int mask, int tag);
    std::shared_ptr<Shape> getShape() override;
    int getCollisionTag() const override;
    int getCollisionFlag() const override;
    int getCollisionMask() const override;
    void start() override;
    void setCollisionFlag(int) override;

private:
    int m_flag;
    int m_mask;
    int m_tag;
    SpriteRenderer* m_renderer;
    Sprite* m_sprite;
    void generateDebugMesh() override;
};

inline void SpriteCollider::setCollisionFlag(int flag) {

    m_flag = flag;
}