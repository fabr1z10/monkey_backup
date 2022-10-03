#include "renderer.h"
#include "../models/sprite.h"
#include "../error.h"
#include "../node.h"

Renderer::Renderer() : Component(), m_multColor(glm::vec4(1.0f)), m_addColor(0.0f), m_rendererTransform(1.f) {}

void Renderer::draw(Shader * s) {
	if (m_model == nullptr || s->getShaderType() != m_model->getShaderType()) {
		return;
	}

	const auto& m = m_node->getWorldMatrix() * m_rendererTransform;
	s->setVec4("mult_color", m_multColor);
	s->setVec4("add_color", m_addColor);
    s->setMat4("model", m);
    m_model->draw(s, 0, 0);

}
//
//void Renderer::innerDraw(Shader * s, const glm::mat4& modelTransform) {
//	m_model->draw(s, modelTransform);
//}

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

bool Renderer::getFlipHorizontal() const {
    return m_rendererTransform[0][0] < 0.f;
}


void Renderer::setTransform(const glm::mat4 &m) {
    m_rendererTransform = m;
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
	if (m_animInfo == nullptr) {
	    GLIB_FAIL("mmh don't know animation: " + anim);
	}
	m_frame = 0;
	m_animation = anim;

}

void SpriteRenderer::start() {
	m_animInfo = m_sprite->getAnimInfo(m_animation);
}


void AnimatedRenderer::start() {
	m_referenceRenderer = dynamic_cast<SpriteRenderer*>(m_node->getParent()->getComponent<Renderer>());
	assert(m_referenceRenderer != nullptr);
}
//
//void AnimatedRenderer::innerDraw(Shader * s, const glm::mat4& m) {
//	auto anim = m_referenceRenderer->getAnimation();
//	auto frame = m_referenceRenderer->getFrame();
//	std::stringstream ss;
//	ss << anim << "_" << frame;
//	m_itemizedModel->innerDraw(s, m, ss.str());
//}
//
void SpriteRenderer::draw(Shader * s) {
    if (m_model == nullptr || s->getShaderType() != m_model->getShaderType()) {
        return;
    }

    const auto& m = m_node->getWorldMatrix() * m_rendererTransform;
    s->setVec4("mult_color", m_multColor);
    s->setVec4("add_color", m_addColor);
    s->setMat4("model", m);

	const auto& a = m_sprite->getFrameInfo(m_animation, m_frame);
	std::cout << "drawing " << a.offset << ", " << a.count << "\n";

    m_model->draw(s, a.offset, a.count);
//	m_sprite->innerDraw(s, modelMatrix, ss.str());
//	//m_sprite->draw(s, nullptr);
	// time to update frame?
	if (m_ticks >= a.ticks) {
		// increment frame. if this animation is
		m_frame++;
		if (m_frame >= m_animInfo->frameCount) {
			m_frame = (m_animInfo->loop ? 0 : m_animInfo->frameCount - 1);
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

std::type_index AnimatedRenderer::getType() {
	return std::type_index(typeid(Renderer));
}

const std::string & SpriteRenderer::getAnimation() const {
    return m_animation;
}

int SpriteRenderer::getFrame() const {
    return m_frame;
}