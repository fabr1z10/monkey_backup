#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "inputtext.h"
#include "../node.h"
#include "../models/rawmodel.h"
#include "../pyhelper.h"



InputText::InputText(const std::string& font, float size, int width, const pybind11::kwargs& args) : Component(), KeyboardListener(),
	m_size(size), m_fontId(font), m_width(width), m_cursorPosition(0) {
	m_cursorSize = dictget<float>(args, "cursor_size", size);
	m_func = args["func"].cast<pybind11::function>();
}

void InputText::start() {
	auto node = std::make_shared<Node>();
	auto vertices = std::vector<float> {
		0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f,
		0.f, m_cursorSize, 0.f, 1.f, 1.f, 1.f, 1.f
	};
	auto elements = std::vector<unsigned> {0,1};
	auto model = std::make_shared<RawModel>(ShaderType::SHADER_COLOR, GL_LINES, vertices, elements);
	node->setModel(model);
	m_cursor = node;
	node->setPosition(m_cursorPosition * m_size, 0.f, 0.f);
	auto textNode = std::make_shared<Node>();
	auto textModel = std::make_shared<Text>(m_fontId, "", m_size);
	textNode->setModel(textModel);
	m_textModel = textModel;
	m_node->add(node);
	m_node->add(textNode);

}

void InputText::keyCallback(GLFWwindow * w, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		// alphanum
		if ((key >= GLFW_KEY_A && key <= GLFW_KEY_Z) || (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) || key == GLFW_KEY_SPACE) {
			m_currentText = m_currentText.substr(0, m_cursorPosition) + static_cast<char>(key) + m_currentText.substr(m_cursorPosition);
			m_textModel->setText(m_currentText);

			// advance cursor
			m_cursorPosition++;
			m_cursor->setPosition(m_cursorPosition * m_size, 0.f, 0.f);
		}
		if (key == GLFW_KEY_LEFT) {
			m_cursorPosition -= 1;
			m_cursorPosition = std::max(m_cursorPosition, 0);
			m_cursor->setPosition(m_cursorPosition * m_size, 0.f, 0.f);
		}
		if (key == GLFW_KEY_RIGHT) {
			m_cursorPosition += 1;
			m_cursorPosition = std::min(m_cursorPosition, static_cast<int>(m_currentText.size()));
			m_cursor->setPosition(m_cursorPosition * m_size, 0.f, 0.f);
		}
		if (key == GLFW_KEY_BACKSPACE) {
			m_currentText = m_currentText.substr(0, m_cursorPosition - 1) + m_currentText.substr(m_cursorPosition);
			m_textModel->setText(m_currentText);

			m_cursorPosition -= 1;
			m_cursorPosition = std::max(m_cursorPosition, 0);
			m_cursor->setPosition(m_cursorPosition * m_size, 0.f, 0.f);
		}
		if (key == GLFW_KEY_ENTER) {
			m_func(m_currentText);
		}

	}
}