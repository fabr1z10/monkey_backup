#include "statemachine.h"
#include "../error.h"
#include "../engine.h"
#include "../runners/scheduler.h"
#include "../pyhelper.h"


State::State(const std::string& id, const pybind11::kwargs& kwargs) : m_id(id), m_scriptId(-1), m_current(false) {
    m_script = dictget<pybind11::function>(kwargs, "script", pybind11::function());
}

void State::init(const pybind11::kwargs& args) {
    m_current = true;
    if (m_script) {
        m_scriptId = m_script(m_sm->getNode()->getId()).cast<long>();
    }
}

void State::end() {
    m_current = false;
    if (m_scriptId != -1) {
        Engine::instance().getRoom()->getRunner<Scheduler>()->kill(m_scriptId);
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