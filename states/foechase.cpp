#include "foechase.h"
#include "../engine.h"
#include "../random.h"




FoeChase2D::FoeChase2D(const std::string& id, const pybind11::kwargs& kwargs) : Walk2D(id, kwargs), m_attackDistance(0.f) {
    m_left = dictget<bool>(kwargs, "left", true);
    m_flipHorizontally = dictget<bool>(kwargs, "flip", false);
    m_right = !m_left;
    m_targetId = dictget<int>(kwargs, "target", -1);
    m_maxSpeed = dictget<float>(kwargs, "speed", 0.f);
    m_leftBound = dictget<float>(kwargs, "left", -std::numeric_limits<float>::infinity());
    m_rightBound = dictget<float>(kwargs, "right", std::numeric_limits<float>::infinity());
    m_chaseRange = kwargs["range"].cast<glm::vec3>();
    // attack information
    if (kwargs.contains("attacks")) {
        for (const auto& k : kwargs["attacks"]) {
            auto state = k["state"].cast<std::string>();
            auto in_range = k["in_range"].cast<bool>();
            auto cprob = k["prob"].cast<float>();
            AttackDetails details {state, in_range};
            m_attackMap[cprob] = details;

        }
    }
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


    glm::vec3 displacement = targetPoint - entityPos;
    if (m_controller->grounded()) {
        m_dynamics->m_velocity.y = 0.0f;
    }
    m_dynamics->m_velocity.y += -m_gravity * dtf;
    auto dl = fabs(displacement.x);
    auto dly = fabs(displacement.y);

    m_inRange = !(dl < attackRange.x || dl > attackRange.y);
    if (randomAttack()) {
        return;
    }

    if (!m_inRange) {
        std::string anim;
        if (entityPos.x < m_leftBound || entityPos.x > m_rightBound) {
            // try to move back to bounds
            m_node->setFlipX(entityPos.x > m_rightBound);
            m_dynamics->m_velocity.x = m_maxSpeed;
            anim= m_walkAnim;
        } else {
            if (dl <= m_chaseRange.x && dly <= m_chaseRange.y) {
                // walk towards player only if within chaserange
                m_animatedRenderer->setAnimation(m_walkAnim);
                //float velo = (dl / dtf) > m_maxSpeed ? m_maxSpeed : (dl /dtf);
                float velo = dl < attackRange.x ? -m_maxSpeed : m_maxSpeed;
                //auto velocity = glm::normalize(displacement) * maxSpeed;
                //m_dynamics->m_velocity.x = fabs(entityPos.x - targetPos.x) > m_attackDistance ? fabs(velocity.x) : -fabs(velocity.x);
                //m_dynamics->m_velocity.z = velocity.z;
                m_dynamics->m_velocity.x = velo;
                anim = m_walkAnim;
            } else {
                m_dynamics->m_velocity.x = 0.f;
                anim = m_idleAnim;
            }
        }

        auto delta = m_dynamics->m_velocity * dtf;
        m_controller->move(delta, false);
        if (m_controller->grounded()) {
            m_animatedRenderer->setAnimation(anim);
        } else {
            m_animatedRenderer->setAnimation(m_jumpAnim);
        }
    } else {
        m_dynamics->m_velocity.x = 0.f;
        auto delta = m_dynamics->m_velocity * dtf;
        m_controller->move(delta, false);
        if (m_controller->grounded()) {
            m_animatedRenderer->setAnimation(m_idleAnim);
        } else {
            m_animatedRenderer->setAnimation(m_jumpAnim);
        }
    }






}

// returns whether attack is performed
bool FoeChase2D::randomAttack() {

    float u = Random::instance().getUniformReal(0.0f, 1.0f);
    auto iter = m_attackMap.lower_bound(u);
    while (iter != m_attackMap.end()) {
        if (u < iter->first) {
            if (!iter->second.inRange || m_inRange) {
                m_sm->setState(iter->second.attackId);
                return true;
            } else {
                break;
            }
        }

        iter++;
    }
    return false;
}