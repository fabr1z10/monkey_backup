#include "foechase.h"
#include "../engine.h"

FoeChase2D::FoeChase2D(const std::string& id, const pybind11::kwargs& kwargs) : Walk2D(id, kwargs), m_attackDistance(0.f) {
    m_left = dictget<bool>(kwargs, "left", true);
    m_flipHorizontally = dictget<bool>(kwargs, "flip", false);
    m_right = !m_left;
    m_targetId = dictget<int>(kwargs, "target", -1);
    m_maxSpeed = dictget<float>(kwargs, "speed", 0.f);

}


void FoeChase2D::setParent(StateMachine * sm) {
    Walk2D::setParent(sm);
    if (m_targetId != -1) {
        m_target = Engine::instance().getNode(m_targetId).get();
    }
    auto range = m_animatedRenderer->getAttackRange();
    m_attackRange = glm::vec2(range.min.x, range.max.x);

}

void FoeChase2D::run(double dt) {
    auto dtf = static_cast<float>(dt);
    auto scale = m_node->getScale();
    // TODO restore attack distance
    //m_thickness = scale.z * m_unscaledThickness;

    // scale the attack range
    auto attackRange = scale * m_attackRange;
    //m_attackDistance = 0.5f * (m_attackRange[0] + m_attackRange[1]);

    auto targetPos = m_target->getWorldPosition();
    auto entityPos = m_node->getWorldPosition();

    float x0 = targetPos.x - m_attackDistance;
    float x1 = targetPos.x + m_attackDistance;
    bool rightOfPlayer = entityPos.x > targetPos.x;
    glm::vec3 targetPoint(rightOfPlayer ? x1 : x0, targetPos.y, targetPos.z);

    m_node->setFlipX(rightOfPlayer);
    auto d = targetPos - entityPos;
    float dx = d.x * (m_node->getFilpX() ? -1.0f : 1.0f);
//    m_inRange = true;//(fabs(d.z) < m_halfThickness) &&
//    //(dx > m_attackRange[0] && dx < m_attackRange[1]);
//    if (m_inRange) {
//        // attack with probability p_attack
//        //std::cout << "qui;";
//        if (randomAttack(dx, scale.x)) {
//            //std::cout << dx << ", " << d.z << ", " << m_halfThickness << " " << m_attackRange[0] << " " << m_attackRange[1] << "\n";
//            return;
//        }
//    }

    // find current displacement
    auto displacement = targetPoint.x - entityPos.x;
    if (m_controller->grounded()) {
        m_dynamics->m_velocity.y = 0.0f;
    }
    m_dynamics->m_velocity.y += -m_gravity * dtf;
    auto dl = fabs(displacement);


    if (dl < attackRange.x || dl > attackRange.y) {
        m_animatedRenderer->setAnimation(m_walkAnim);
        //float velo = (dl / dtf) > m_maxSpeed ? m_maxSpeed : (dl /dtf);
        float velo = dl < attackRange.x ? - m_maxSpeed : m_maxSpeed;
        //auto velocity = glm::normalize(displacement) * maxSpeed;
        //m_dynamics->m_velocity.x = fabs(entityPos.x - targetPos.x) > m_attackDistance ? fabs(velocity.x) : -fabs(velocity.x);
        //m_dynamics->m_velocity.z = velocity.z;
        m_dynamics->m_velocity.x = velo;
        auto delta = m_dynamics->m_velocity * dtf;
        m_controller->move(delta, false);
    } else {
        m_dynamics->m_velocity = glm::vec3(0.0f);
        auto delta = m_dynamics->m_velocity * dtf;
        m_controller->move(delta, false);
        if (m_controller->grounded()) {
            m_animatedRenderer->setAnimation(m_idleAnim);
        } else {
            m_animatedRenderer->setAnimation(m_walkAnim);
        }
    }


    return;



}