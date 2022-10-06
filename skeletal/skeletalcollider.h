#pragma once

#include "../components/collider.h"
#include "skeletalrenderer.h"

class SkeletalCollider : public Collider {
public:
    SkeletalCollider(int flag, int mask, int tag);
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
    SkeletalRenderer* m_renderer;
    SkeletalModel* m_model;
    void generateDebugMesh() override;
};

inline void SkeletalCollider::setCollisionFlag(int flag) {

    m_flag = flag;
}

class SkeletalColliderRenderer : public Renderer {
public:
    SkeletalColliderRenderer() = default;
    void draw(Shader * s) override;
    void start() override;
    std::type_index getType() override;
private:
    SkeletalRenderer* m_reference;
    //void innerDraw(Shader*, const glm::mat4&) override;
    SkeletalModel* m_skeletalModel;
};

