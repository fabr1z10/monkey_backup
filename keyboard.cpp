#include "keyboard.h"


// all key events go here
void Keyboard::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//	// don't handle key events for disable keys until mods==16 (programmatically for demo-mode)
//	if (key < 512 && mods != 16 && (m_enabled[key] != 1)) {
//		return;
//	}
//	if (mods==16) {
//		PressKey(key, action);
//	}
//	for (auto& listener : m_keyboardListeners)
//		listener->KeyCallback(window, key, scancode, action, mods);
}