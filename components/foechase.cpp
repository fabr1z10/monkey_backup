#include "foechase.h"
#include "../engine.h"
#include "../random.h"

Hit2D::Hit2D(const std::string& id, const pybind11::kwargs& kwargs) : State(id, kwargs) {
    m_speed = kwargs["speed"].cast<float>();
    m_anim = kwargs["anim"].cast<std::string>();
    m_gravity = dictget<float>(kwargs, "gravity", 0.0f);
    m_timeToStop = kwargs["time"].cast<float>();
    m_exitState = kwargs["exit_state"].cast<std::string>();
}

void Hit2D::setParent(StateMachine * sm) {
    State::setParent(sm);
    m_controller = dynamic_cast<Controller2D*>(m_sm->getNode()->getComponent<Controller>());
    assert(m_controller != nullptr);
    m_dynamics = m_sm->getNode()->getComponent<Dynamics>();
    assert(m_dynamics != nullptr);
    m_animatedRenderer = dynamic_cast<AnimatedRenderer*>(m_sm->getNode()->getComponent<Renderer>());

}

void Hit2D::init(const pybind11::kwargs &args) {
    // where is the hitter relative to the hit
    auto xOther = args["x"].cast<float>();
    float xSelf = m_sm->getNode()->getWorldPosition().x;
    m_actualSpeed = ((xSelf >= xOther) == m_sm->getNode()->getFilpX()) ? -m_speed : m_speed;
    State::init(args);
    m_animatedRenderer->setAnimation(m_anim);
    m_ax = -m_actualSpeed / m_timeToStop;
    m_dynamics->m_velocity = glm::vec3(m_actualSpeed, 0.f, 0.f);
}

void Hit2D::run(double dt) {
    auto dtf = static_cast<float>(dt);
    //m_dynamics->m_velocity = glm::vec3(m_actualSpeed, 0.f, 0.f);
    auto delta = m_dynamics->m_velocity * dtf;
    m_dynamics->m_acceleration = glm::vec3(m_ax,  -m_gravity, 0.f);
    //m_dynamics->update(dt);

    m_controller->move(delta, false);
    // when horizontal velocity is 0 -> exit
    if (fabs(m_dynamics->m_velocity.x) < 0.001f) {
        m_sm->setState(m_exitState);
    }

}


FoeChase2D::FoeChase2D(const std::string& id, const pybind11::kwargs& kwargs) : Walk2D(id, kwargs), m_attackDistance(0.f) {
    m_left = dictget<bool>(kwargs, "left", true);
    m_flipHorizontally = dictget<bool>(kwargs, "flip", false);
    m_right = !m_left;
    m_targetId = dictget<int>(kwargs, "target", -1);
    m_maxSpeed = dictget<float>(kwargs, "speed", 0.f);


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
    auto displacement = targetPoint.x - entityPos.x;
    if (m_controller->grounded()) {
        m_dynamics->m_velocity.y = 0.0f;
    }
    m_dynamics->m_velocity.y += -m_gravity * dtf;
    auto dl = fabs(displacement);
    m_inRange = !(dl < attackRange.x || dl > attackRange.y);
    if (randomAttack()) {
        return;
    }

    if (!m_inRange) {
        m_animatedRenderer->setAnimation(m_walkAnim);
        //float velo = (dl / dtf) > m_maxSpeed ? m_maxSpeed : (dl /dtf);
        float velo = dl < attackRange.x ? - m_maxSpeed : m_maxSpeed;
        //auto velocity = glm::normalize(displacement) * maxSpeed;
        //m_dynamics->m_velocity.x = fabs(entityPos.x - targetPos.x) > m_attackDistance ? fabs(velocity.x) : -fabs(velocity.x);
        //m_dynamics->m_velocity.z = velocity.z;
        m_dynamics->m_velocity.x = velo;
        auto delta = m_dynamics->m_velocity * dtf;
        m_controller->move(delta, false);
        if (m_controller->grounded()) {
            m_animatedRenderer->setAnimation(m_walkAnim);
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
        if (!iter->second.inRange || m_inRange) {
            m_sm->setState(iter->second.attackId);
            return true;
        }
        iter++;
    }
    return false;
}