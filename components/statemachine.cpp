#include "statemachine.h"
#include "../error.h"
#include "../engine.h"
#include "../runners/scheduler.h"
#include "../pyhelper.h"
#include "collider.h"


State::State(const std::string& id, const pybind11::kwargs& kwargs) : m_id(id), m_scriptId(-1), m_current(false) {
    m_script = dictget<pybind11::function>(kwargs, "script", pybind11::function());

    if (kwargs.contains("keys")) {
        auto keys = kwargs["keys"].cast<pybind11::dict>();
        for (const auto& key : keys) {
            auto keyId = key.first.cast<int>();
            auto callback = key.second.cast<pybind11::function>();
            m_keyCallbacks.insert(std::make_pair(keyId, callback));
        }
    }

    m_collisionFlag = dictget<int>(kwargs, "collision_flag", -1);
    m_collisionMask = dictget<int>(kwargs, "collision_mask", -1);
    m_collisionTag = dictget<int>(kwargs, "collision_tag", -1);
    m_overrideCollision = m_collisionFlag != -1 || m_collisionMask != -1 || m_collisionTag != -1;


}

void State::keyCallback(int key) {
    auto it = m_keyCallbacks.find(key);
    if (it != m_keyCallbacks.end()) {
        it->second();
    }

}

void StateMachine::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (m_currentState != nullptr) m_currentState->keyCallback(key);
    }
}


void State::init(const pybind11::kwargs& args) {
    m_current = true;
    if (m_script) {
        m_scriptId = m_script(m_sm->getNode()->getId()).cast<long>();
    }

    if (m_overrideCollision) {
        auto* collider = m_sm->getNode()->getComponent<Collider>();
        if (m_collisionFlag != -1) {
            m_backupFlag = collider->getCollisionFlag();
            collider->setCollisionFlag(m_collisionFlag);
        }
        if (m_collisionMask != -1) {
            m_backupMask = collider->getCollisionMask();
            collider->setCollisionMask(m_collisionMask);
        }
        if (m_collisionTag != -1) {
            m_backupTag = collider->getCollisionTag();
            collider->setCollisionTag(m_collisionTag);
        }
    }



}

void State::end() {
    m_current = false;
    if (m_scriptId != -1) {
        Engine::instance().getRoom()->getRunner<Scheduler>()->kill(m_scriptId);
    }
    if (m_overrideCollision) {
        // restore previous collision data
        auto* collider = m_sm->getNode()->getComponent<Collider>();
        if (m_collisionFlag != -1) {
            collider->setCollisionFlag(m_backupFlag);
        }
        if (m_collisionMask != -1) {
            collider->setCollisionMask(m_backupMask);
        }
        if (m_collisionTag != -1) {
            collider->setCollisionTag(m_backupTag);
        }
    }
}

void StateMachine::start() {
	for (const auto& s : m_states) {
		s.second->setParent(this);
	}
	if (!m_initialState.empty()) {
		setState(m_initialState, m_args);
	}
}



void StateMachine::setState(const std::string & state, const pybind11::kwargs& args) {
	auto it = m_states.find(state);
	if (it == m_states.end()) {
		GLIB_FAIL("Don't know state: " + state);
	}
	if (m_currentState) {
	    m_currentState->end();
	}
	it->second->init(args);
	m_currentState = it->second;
}

void StateMachine::update(double dt) {
	if (m_currentState != nullptr) {
		m_currentState->run(dt);
	}
}