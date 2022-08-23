#pragma once

#include <pybind11/pytypes.h>
#include "../component.h"
#include "../keylistener.h"
#include "../models/text.h"

class InputText : public Component, public KeyboardListener {
public:
	InputText(const std::string& font, float size, int width, const pybind11::kwargs&);
	void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;
	void start() override;
private:
	int m_cursorPosition;
	int m_width;
	float m_size;
	std::string m_fontId;
	std::string m_currentText;
	float m_cursorSize;
	std::shared_ptr<Text> m_textModel;
	std::shared_ptr<Node> m_cursor;
	pybind11::function m_func;
};