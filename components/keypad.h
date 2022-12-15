#pragma once

#include "../component.h"
#include <pybind11/pybind11.h>
#include <glm/glm.hpp>

class KeyPad : public Component {
public:
	KeyPad(const pybind11::kwargs& args);
	bool left() const;
	bool right() const;
	bool up() const;
	bool down() const;
	bool button1() const;
	bool button2() const;
	bool button3() const;
	bool button4() const;
	bool jump() const;
	bool check(unsigned) const;
	// returns true if any direction key is down
	bool directionKey() const;
	// returns whether left or right is down
	bool lor() const;
	// returns whether up or down is down
	bool uod() const;
	bool flipHorizontal() const;
	void update(double) override;
	int back() const;
protected:
	bool m_fliph;
	int m_back;
	unsigned m_jumpKey;
	unsigned m_keys;
};

inline int KeyPad::back() const {
    return m_back;
}

inline bool KeyPad::check(unsigned int value) const {
	return m_keys & value;
}

inline bool KeyPad::flipHorizontal() const {
	return m_fliph;
}

inline bool KeyPad::jump() const {
	return m_keys & m_jumpKey;
}

inline bool KeyPad::lor() const {
	return m_keys & 0b11u;
}

inline bool KeyPad::uod() const {
	return m_keys & 0x0Cu;
}


inline bool KeyPad::left() const {
	return m_keys & 0x01u;
}


inline bool KeyPad::right() const{
	return m_keys & 0x02u;
}

inline bool KeyPad::up() const {
	return m_keys & 0x04u;
}

inline bool KeyPad::down() const {
	return m_keys & 0x08u;
}

inline bool KeyPad::directionKey() const {
	return m_keys & 0x0Fu;
}

inline bool KeyPad::button1() const {
	return m_keys & 0x10u;
}

inline bool KeyPad::button2() const {
	return m_keys & 0x20u;
}

inline bool KeyPad::button3() const {
	return m_keys & 0x40u;
}

inline bool KeyPad::button4() const {
	return m_keys & 0x80u;
}


class UserKeyPad : public KeyPad {
public:
	explicit UserKeyPad(const pybind11::kwargs& args);
	void update(double) override;
	std::type_index getType() override;

private:
	unsigned m_leftKey;
	unsigned m_rightKey;
	unsigned m_upKey;
	unsigned m_downKey;
	unsigned m_btn1;
	unsigned m_btn2;
	unsigned m_btn3;
	unsigned m_btn4;


};

class AIKeyPad : public KeyPad {
public:
	explicit AIKeyPad(const pybind11::kwargs& args);
	void start() override;
	void update(double) override;
	std::type_index getType() override;

private:
    int m_targetId;
    Node* m_target;
    glm::vec2 m_attackRange;
    float m_targetDistance;
    float m_attackProbability;

};
