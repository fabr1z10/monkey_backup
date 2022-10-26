#pragma once

#include "collider.h"
#include "renderer.h"

class SpriteCollider : public Collider {
public:
    SpriteCollider(int flag, int mask, int tag, const pybind11::kwargs& args);
    std::shared_ptr<Shape> getShape() override;
    int getCollisionTag() const override;
    int getCollisionFlag() const override;
    int getCollisionMask() const override;
    void start() override;
    void setCollisionFlag(int) override;
    void update(double) override;

private:
    int m_flag;
    int m_mask;
    int m_tag;
    int m_castMask;
    int m_castTag;
    SpriteRenderer* m_renderer;
    Sprite* m_sprite;
    void generateDebugMesh() override;
};

inline void SpriteCollider::setCollisionFlag(int flag) {

    m_flag = flag;
}

class SpriteColliderRenderer : public Renderer {
public:
    SpriteColliderRenderer() = default;
    void draw(Shader * s) override;
    void start() override;
    std::type_index getType() override;
private:
    SpriteRenderer* m_reference;
    Sprite* m_sprite;
};

