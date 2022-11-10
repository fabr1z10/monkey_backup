#include "smartcollider.h"
#include "../node.h"
#include "../engine.h"
#include "../pyhelper.h"
#include "../runners/collisionengine.h"
#include "../models/sprite.h"

SpriteCollider::SpriteCollider(int flag, int mask, int tag, const pybind11::kwargs& args) : Collider(),
m_sprite(nullptr), m_renderer(nullptr), m_flag(flag), m_mask(mask), m_tag(tag) {
    m_castMask = dictget<int>(args, "cast_mask", 0);
    m_castTag = dictget<int>(args, "cast_tag", 0);

}

void SpriteCollider::start() {
    // a node that has a spritecollider needs to have a sprite model (and a sprite renderer)
    m_renderer = dynamic_cast<SpriteRenderer*>(m_node->getComponent<Renderer>());
    assert(m_renderer != nullptr);

    m_sprite = m_renderer->getSprite();
    m_staticBounds = m_sprite->getStaticBounds();
    Collider::start();
//    auto& engine = Engine::instance();
//    auto collDebug = pyget<bool>(engine.getConfig().attr("settings"),"debug_collision", false);
//    if (collDebug) {
//        generateDebugMesh();
//    }

}

void SpriteCollider::update(double) {
    // check if current (anim, frame) has a shape to cast
    auto animId = m_renderer->getAnimation();
    auto frame = m_renderer->getFrame();
    auto box = m_sprite->getShapeCast(animId, frame);
    bool hit = false;
    if (box != nullptr) {
        auto t = m_node->getWorldMatrix();
        auto e = m_engine->shapeCast(box.get(), t, m_castMask, true);
        if (!e.empty()) {
            hit = true;
            m_engine->processCollisions(e, m_node, m_castTag);
        }
    }
    if (!hit) {
        // m_lastHit = nullptr;
    }




}


std::shared_ptr<Shape> SpriteCollider::getShape() {
    return m_sprite->getShape(m_renderer->getAnimation(), m_renderer->getFrame());
}

int SpriteCollider::getCollisionTag() const {
    return m_tag;
}

int SpriteCollider::getCollisionFlag() const {
    return m_sprite->hasCollision(m_renderer->getAnimation()) ? m_flag : 0;
}

int SpriteCollider::getCollisionMask() const {
    return m_sprite->hasCollision(m_renderer->getAnimation()) ? m_mask : 0;
}

void SpriteCollider::generateDebugMesh() {
	if (m_debugNode != nullptr) {
		m_debugNode->remove();
	}
    auto model = m_sprite->generateDebugModel();

    auto node = std::make_shared<Node>();
    node->setModel(model);
    auto renderer = std::make_shared<SpriteColliderRenderer>();
    renderer->setModel(model);
    node->addComponent(renderer);
    m_node->add(node);
    m_debugNode = node.get();
}


std::type_index SpriteColliderRenderer::getType() {
    return std::type_index(typeid(Renderer));
}

void SpriteColliderRenderer::start() {
    Renderer::start();
    m_reference = dynamic_cast<SpriteRenderer*>(m_node->getParent()->getComponent<Renderer>());
    m_sprite = m_reference->getSprite();
    assert(m_reference!=nullptr);
}

void SpriteColliderRenderer::draw(Shader * s) {
    if (m_model == nullptr || s->getShaderType() != ShaderType::SHADER_COLOR) {
        return;
    }
    const auto& m = m_node->getWorldMatrix() ;
    s->setVec4("mult_color", m_multColor);
    s->setVec4("add_color", m_addColor);
    s->setMat4("model", m);
    const auto& currentAnim = m_reference->getAnimation();
    int currentFrame = m_reference->getFrame();
    auto data = m_sprite->getDebugShape(currentAnim, currentFrame);
    auto dataA = m_sprite->getDebugAttackShape(currentAnim, currentFrame);
    if (data.first != -1) {
        m_model->draw(s, data.first, data.second);
    }
    if (dataA.first != -1) {
        m_model->draw(s, dataA.first, dataA.second);
    }
//    auto p = m_skeletalModel->getDebugShape(currentAnim, 0);
//    if (p.first != -1) {
//        m_model->draw(s, p.first, p.second);
//    }
//    auto time = m_reference->getAnimationTime();
//    auto anim = m_skeletalModel->getAnimation(currentAnim);
//    if (anim != nullptr) {
//        int attackId = anim->getAttack(time);
//        if (attackId != -1) {
//            auto p = m_skeletalModel->getDebugShape(currentAnim, attackId + 1);
//            if (p.first != -1) {
//                m_model->draw(s, p.first, p.second);
//            }
//        }
//    }




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