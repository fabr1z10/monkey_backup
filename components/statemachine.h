#pragma once

#include "../component.h"
#include <memory>
#include <unordered_map>



class State {
public:
	State() {}
	virtual ~State() = default;
	virtual void Init (const ITab& d) = 0;
	virtual void Run (double) = 0;
	virtual void End () = 0;
	// This is called only once when the statemachine begins, so PUT your initialization code here!
	virtual void AttachStateMachine(StateMachine*);
	virtual bool KeyListener (int);
	std::string getId() const;
	void AddKey (int, std::shared_ptr<StateAction>);
	virtual void onCollide();
protected:
	std::string m_id;
	StateMachine* m_sm;
	std::unordered_map<int, std::shared_ptr<StateAction>> m_actions;
};

inline std::string State::getId() const {
	return m_id;
}

//!  A state machine.
/*!
 A state machine is a component that allows an entity to have a different
 behavior based on what the current state is. A state machine has a set of
 states, of which only one is active at any given time. It provides methods
 to get the current state, and change the state to a new one.
 */
class StateMachine : public Component {
public:
	StateMachine() : m_currentState(nullptr), Component() {}
	StateMachine(const std::string& initialState) : m_initialState(initialState), m_currentState(nullptr) {}
	void start () override {}
	void update(double) override;
	std::shared_ptr<State> getState() const;
	void setState(const std::string&);
	virtual void Refresh () {}
	void addState(std::shared_ptr<State> state);
protected:
	std::shared_ptr<State> m_currentState;
	std::string m_initialState;
	std::unordered_map<std::string, std::shared_ptr<State>> m_states;
};

inline void StateMachine::addState(std::shared_ptr<State> state) {
	m_states.insert(std::make_pair(state->getId(), state));
}

inline std::shared_ptr<State> StateMachine::getState() const {
	return m_currentState;
}


