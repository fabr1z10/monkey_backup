#include "jumpattack.h"
#include "../shapes/aabb.h"
#include "../node.h"
#include "../runners/collisionengine.h"
#include "../engine.h"

JumpAttack::JumpAttack(const std::string& id, const pybind11::kwargs& kwargs) : State(id, kwargs) {

    m_jumpUp = dictget<std::string>(kwargs, "jump_up", "jump_up");
    m_jumpDown = dictget<std::string>(kwargs, "jump_down", "jump_down");
    m_gravity = kwargs["gravity"].cast<float>();
    m_peakHeight = kwargs["peak_height"].cast<float>();
    m_downSpeed = kwargs["down_speed"].cast<float>();
    m_targetId = dictget<int>(kwargs, "target", -1);
    m_castTag = kwargs["cast_tag"].cast<int>();
    m_castMask = kwargs["cast_mask"].cast<int>();
    m_exitState = kwargs["exit_state"].cast<std::string>();
    m_hitState = kwargs["hit_state"].cast<std::string>();

}

void JumpAttack::setParent(StateMachine * sm) {
    State::setParent(sm);
    if (m_targetId != -1) {
        m_target = Engine::instance().getNode(m_targetId).get();
    }
    m_controller = dynamic_cast<Controller2D*>(m_sm->getNode()->getComponent<Controller>());
    assert(m_controller != nullptr);
    m_dynamics = m_sm->getNode()->getComponent<Dynamics>();
    assert(m_dynamics != nullptr);
    m_animatedRenderer = dynamic_cast<AnimatedRenderer*>(m_sm->getNode()->getComponent<Renderer>());

    m_engine = Engine::instance().getRoom()->getRunner<CollisionEngine>();
}

void JumpAttack::init(const pybind11::kwargs& args) {
    auto entityPos = m_sm->getNode()->getWorldPosition();
    auto targetPos = m_target->getWorldPosition();
    auto delta = targetPos - entityPos;
    delta.y = 0.0f;
    auto dx = glm::length(delta);
    std::cerr << " horizontal spacing = " << dx << "\n";
    // vertical speed
    float vy = sqrt(2.0f * m_gravity * m_peakHeight);
    // horizontal speed
    float vx = (dx * m_gravity) / vy;
    glm::vec3 velocity = vx * glm::normalize(delta);
    velocity.y = vy;
    velocity.x = fabs(velocity.x);
    m_animatedRenderer->setAnimation(m_jumpUp);
    m_dynamics->m_velocity = velocity;
    m_sm->getNode()->setFlipX(delta.x < 0);
    m_hit = false;
}

void JumpAttack::run(double dt) {
    auto dtf = static_cast<float>(dt);

    glm::vec3 acceleration(0.f, -m_gravity, 0.f);

    m_dynamics->m_velocity += acceleration * dtf;
    auto delta = m_dynamics->m_velocity * dtf;
    m_controller->move(delta, false);
    if (m_dynamics->m_velocity.y <= 0.0f) {
        m_animatedRenderer->setAnimation(m_jumpDown);
        m_dynamics->m_velocity.x = 0.0f;
        m_dynamics->m_velocity.z = 0.0f;
        m_dynamics->m_velocity.y = -m_downSpeed;
    }

    if (m_dynamics->m_velocity.y < 0.0f) {
        // test collision
        auto size = m_controller->getSize();
        auto r = AABB(-0.5f * size.x, 0.5f * size.x, 0.f, size.y);
        auto t = m_sm->getNode()->getWorldMatrix();
        auto e = m_engine->shapeCast(&r, t, m_castMask, true);
        m_engine->processCollisions(e, m_sm->getNode(), m_castTag);
        if (e.size() > 0) {
        	m_hit = true;
            std::cerr << "FUCKING HIT!!!!\n";
//            auto rm = m_engine->GetResponseManager();
//            if (rm == nullptr) {
//                std::cerr << "no handler!\n";
//            } else {
//                auto object = e.entity->GetObject();
//                auto handler = rm->GetHandler(m_castTag, e.entity->GetCollisionTag());
//                if (handler.response != nullptr) {
//                    std::cerr << "FOUND RESPONSE\n";
//                    //m_stateMachine->getCurrentState()->onCollide();
//                    if (handler.flip) {
//                        handler.response->onStart(object, m_entity, e.report);
//                    } else {
//                        handler.response->onStart(m_entity, object, e.report);
//                    }
//                }
//            }
        }

    }
    if (m_controller->grounded()) {
        if (m_hit) {
            m_sm->setState(m_hitState);
        } else {
            m_sm->setState(m_exitState);
        }
    }
}