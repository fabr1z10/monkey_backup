#include "renderer.h"

void Renderer::draw(Shader * s) {
	if (m_model == nullptr || s->getShaderType() != m_model->getShaderType()) {
		return;
	}

	m_model->draw(s, 0, 0);
}

void Renderer::setModel(std::shared_ptr<Model> model) {
	m_model = model;
}