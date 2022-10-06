#include "skeletalcollider.h"
#include "../node.h"


SkeletalCollider::SkeletalCollider(int flag, int mask, int tag) : Collider(),
    m_model(nullptr), m_renderer(nullptr), m_flag(flag), m_mask(mask), m_tag(tag) {}


std::shared_ptr<Shape> SkeletalCollider::getShape() {

    std::string anim = m_renderer->getAnimation();
    return m_model->getShape(anim);
}


void SkeletalCollider::start() {
    // a node that has a spritecollider needs to have a sprite model (and a sprite renderer)
    m_renderer = dynamic_cast<SkeletalRenderer*>(m_node->getComponent<Renderer>());
    assert(m_renderer != nullptr);

    m_model = m_renderer->getSkeletalModel();
    m_staticBounds = m_model->getStaticBounds();
    Collider::start();

}

int SkeletalCollider::getCollisionTag() const {
    return m_tag;
}

int SkeletalCollider::getCollisionFlag() const {
    return m_model->hasCollision(m_renderer->getAnimation()) ? m_flag : 0;
}

int SkeletalCollider::getCollisionMask() const {
    return m_model->hasCollision(m_renderer->getAnimation()) ? m_mask : 0;
}

void SkeletalCollider::generateDebugMesh() {

    if (m_debugNode != nullptr) {
        m_debugNode->remove();
    }
    auto model = m_model->generateDebugModel();

    auto node = std::make_shared<Node>();
    node->setModel(model);
    auto renderer = std::make_shared<SkeletalColliderRenderer>();
    renderer->setModel(model);
    node->addComponent(renderer);
    m_node->add(node);
    m_debugNode = node.get();
}

void SkeletalColliderRenderer::start() {
    Renderer::start();
    m_reference = dynamic_cast<SkeletalRenderer*>(m_node->getParent()->getComponent<Renderer>());
    m_skeletalModel = m_reference->getSkeletalModel();
    assert(m_reference!=nullptr);
}


std::type_index SkeletalColliderRenderer::getType() {
    return std::type_index(typeid(Renderer));
}


void SkeletalColliderRenderer::draw(Shader * s) {
    if (m_model == nullptr || s->getShaderType() != ShaderType::SHADER_COLOR) {
        return;
    }
    const auto& m = m_node->getWorldMatrix() ;
    s->setVec4("mult_color", m_multColor);
    s->setVec4("add_color", m_addColor);
    s->setMat4("model", m);
    const auto& currentAnim = m_reference->getAnimation();
    auto p = m_skeletalModel->getDebugShape(currentAnim, 0);
    if (p.first != -1) {
        m_model->draw(s, p.first, p.second);
    }
    auto time = m_reference->getAnimationTime();
    auto anim = m_skeletalModel->getAnimation(currentAnim);
    if (anim != nullptr) {
        int attackId = anim->getAttack(time);
        if (attackId != -1) {
            auto p = m_skeletalModel->getDebugShape(currentAnim, attackId + 1);
            if (p.first != -1) {
                m_model->draw(s, p.first, p.second);
            }
        }
    }




//    const auto& m = m_node->getWorldMatrix() * m_rendererTransform;
//    s->setVec4("mult_color", m_multColor);
//    s->setVec4("add_color", m_addColor);
//    s->setMat4("model", m);
//
//    const auto& a = m_sprite->getFrameInfo(getAnimation(), getFrame());
//    //std::cout << "drawing " << a.offset << ", " << a.count << "\n";
//
//    m_model->draw(s, a.offset, a.count);
////	m_sprite->innerDraw(s, modelMatrix, ss.str());
////	//m_sprite->draw(s, nullptr);
//    // time to update frame?
//    if (m_ticks >= a.ticks) {
//        // increment frame. if this animation is
//        m_frame++;
//        if (m_frame >= m_animInfo->frameCount) {
//            m_frame = (m_animInfo->loop ? 0 : m_animInfo->frameCount - 1);
//        }
//        m_ticks = 0;
//    } else {
//        // if it's not time to update frame, increment current frame length
//        m_ticks++;
//    }

}