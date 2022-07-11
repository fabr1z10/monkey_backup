#pragma once

struct GLFWwindow;

class KeyboardListener {
public:
	KeyboardListener();
	virtual~ KeyboardListener();
	virtual void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) = 0;
};