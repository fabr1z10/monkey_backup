#include "walk2d.h"
#include "../pyhelper.h"
#include "../node.h"
#include "../util.h"



//void Walk2D::init() {
//
//}

Walk2D::Walk2D(const std::string& id, const pybind11::kwargs& kwargs) : State(id, kwargs) {
	m_gravity = dictget<float>(kwargs, "gravity", 0.0f);
	m_jumpHeight = kwargs["jump_height"].cast<float>();
    m_timeToJumpApex = kwargs["time_to_jump_apex"].cast<float>();
    m_jumpVelocity = (m_jumpHeight + 0.5f * m_gravity * m_timeToJumpApex * m_timeToJumpApex) / m_timeToJumpApex;
	m_maxSpeed = kwargs["speed"].cast<float>();
	m_accelerationTime = dictget<float>(kwargs, "acc_time", 0.1f);
	m_acceleration = m_maxSpeed / m_accelerationTime;
    m_idleAnim = dictget<std::string>(kwargs, "idle_anim", "idle");
    m_walkAnim = dictget<std::string>(kwargs, "walk_anim", "walk");
    m_jumpAnim = dictget<std::string>(kwargs, "jump_anim", "jump");
	std::cout << "gravity = " << m_gravity << "\n";
}

void Walk2D::init(const pybind11::kwargs &args) {
    State::init(args);
    auto dir = dictget<float>(args, "dir", -1.0f);
    m_left = dir < 0.0f;
    m_right = !m_left;

}

void Walk2D::setParent(StateMachine * sm) {
	State::setParent(sm);
	m_node = sm->getNode();

	m_controller = dynamic_cast<Controller2D*>(m_sm->getNode()->getComponent<Controller>());
	assert(m_controller != nullptr);

	m_dynamics = m_sm->getNode()->getComponent<Dynamics>();
	assert(m_dynamics != nullptr);

	m_animatedRenderer = dynamic_cast<AnimatedRenderer*>(m_node->getComponent<Renderer>());
}


void Walk2D::run(double dt) {
	auto dtf = static_cast<float>(dt);

	control();



    if (m_controller->grounded()) {
        if (m_up) {
            m_dynamics->m_velocity.y = m_jumpVelocity;
        } else {
            m_dynamics->m_velocity.y = 0.0f;
        }
    } else {
        // bump head
        if (m_controller->ceiling()) {
            m_dynamics->m_velocity.y = 0;
        }

    }


	glm::vec3 a(0.0f);
	a.y = -m_gravity;

	if (m_left || m_right) {
		a.x = m_acceleration; // (m_left ? -1.f : 1.f) * m_acceleration;
	} else {
		// apply deceleration only if velocity above threshold
		if (fabs(m_dynamics->m_velocity.x) > 0.1f) {
			a.x = - m_acceleration;
		} else {
			a.x = 0.0f;
			m_dynamics->m_velocity.x = 0.0f;
		}
	}

	m_dynamics->m_velocity += a * dtf;

	// limit horizontal vel to max speed
	if (m_left || m_right) {
		if (fabs(m_dynamics->m_velocity.x) > m_maxSpeed) {
			m_dynamics->m_velocity.x = signf(m_dynamics->m_velocity.x) * m_maxSpeed;
		}
	}


	auto delta = m_dynamics->m_velocity * dtf;

	m_controller->move(delta, false);

	// update animation, if we have a sprite renderer
	if (m_animatedRenderer) {
	    if (m_controller->grounded()) {
	        if (fabs(m_dynamics->m_velocity.x) < 0.1f) {
                m_animatedRenderer->setAnimation(m_idleAnim);
	        } else {
                m_animatedRenderer->setAnimation(m_walkAnim);
	        }
	    } else {
            m_animatedRenderer->setAnimation(m_jumpAnim);
	    }

	}
}