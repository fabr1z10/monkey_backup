#include "renderer.h"
#include "../models/sprite.h"

Renderer::Renderer() : Component(), m_multColor(glm::vec4(1.0f)), m_addColor(0.0f), m_rendererTransform(1.f) {}

void Renderer::draw(Shader * s) {
	if (m_model == nullptr || s->getShaderType() != m_model->getShaderType()) {
		return;
	}
	s->setVec4("mult_color", m_multColor);
	s->setVec4("add_color", m_addColor);
	innerDraw(s);

}

void Renderer::innerDraw(Shader * s) {
	m_model->draw(s, 0, 0);
}

void Renderer::setModel(std::shared_ptr<Model> model) {
	m_model = model;
}

void Renderer::setMultColor(glm::vec4 multColor) {
	m_multColor = multColor;
}

void Renderer::setAddColor(glm::vec4 addColor) {
	m_addColor = addColor;
}

void Renderer::flipHorizontal(bool value) {
	m_rendererTransform[0] = abs(m_rendererTransform[0]) * (value ? -1.f : 1.f);
}

SpriteRenderer::SpriteRenderer(const std::string& anim) : m_animation(anim), m_frame(0), m_ticks(0) {

}

void SpriteRenderer::setModel(std::shared_ptr<Model> model) {
	Renderer::setModel(model);
	m_sprite = std::dynamic_pointer_cast<Sprite>(model);
}

void SpriteRenderer::setAnimation(const std::string& anim) {
	if (anim == m_animation) {
		return;
	}

	//m_complete = false;
	m_animInfo = m_sprite->getAnimInfo(anim);
	m_frame = 0;
	m_animation = anim;

}

void SpriteRenderer::start() {
	m_animInfo = m_sprite->getAnimInfo(m_animation);
}


void SpriteRenderer::innerDraw(Shader * s) {
	const auto& a = m_sprite->getFrameInfo(m_animation, m_frame);
	m_sprite->draw(s, a.offset, a.count);
	// time to update frame?
	if (m_ticks >= a.ticks) {
		// increment frame. if this animation is
		m_frame++;
		if (m_frame >= m_animInfo->frames) {
			m_frame = (m_animInfo->loop ? 0 : m_animInfo->frames - 1);
		}
		m_ticks = 0;
	} else {
		// if it's not time to update frame, increment current frame length
		m_ticks++;
	}
}

std::type_index SpriteRenderer::getType() {
	return std::type_index(typeid(Renderer));
}

const std::string & SpriteRenderer::getAnimation() const {
    return m_animation;
}

int SpriteRenderer::getFrame() const {
    return m_frame;
}