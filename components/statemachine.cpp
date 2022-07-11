#include "statemachine.h"
#include "../error.h"

void StateMachine::start() {
	for (const auto& s : m_states) {
		s.second->setParent(this);
	}
	if (!m_initialState.empty()) {
		setState(m_initialState);
	}
}

void StateMachine::setState(const std::string & state) {
	auto it = m_states.find(state);
	if (it == m_states.end()) {
		GLIB_FAIL("Don't know state: " + state);
	}
	it->second->init();
	m_currentState = it->second;
}

void StateMachine::update(double dt) {
	if (m_currentState != nullptr) {
		m_currentState->run(dt);
	}
}