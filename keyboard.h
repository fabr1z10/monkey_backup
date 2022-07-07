#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unordered_set>
#include "keylistener.h"

class Keyboard {
public:
	Keyboard() = default;
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
private:
	int m_keyState[512];
	int m_enabled[512];
	std::unordered_set<KeyboardListener*> m_keyboardListeners;

};

//inline void Keyboard::addListener(KeyboardListener* listener) {
//	m_keyboardListeners.insert(listener);
//}
//
//inline void Keyboard::removeListener(KeyboardListener* listener) {
//	m_keyboardListeners.erase(listener);
//}
