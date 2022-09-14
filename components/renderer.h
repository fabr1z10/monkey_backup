#pragma once

#include "../component.h"
#include "../model.h"
#include "../models/sprite.h"


class Renderer : public Component {
public:
	Renderer();
	void draw(Shader*);
	std::shared_ptr<Model> getModel();
	virtual void setModel(std::shared_ptr<Model>);
	void setMultColor(glm::vec4);
	void setAddColor(glm::vec4);
	const glm::mat4& getRendererTransform() const;
	void flipHorizontal(bool);
	bool getFlipHorizontal() const;
protected:
	virtual void innerDraw(Shader*, const glm::mat4& modelTransform) ;
private:
	std::shared_ptr<Model> m_model;
	glm::vec4 m_multColor;
	glm::vec4 m_addColor;
	glm::mat4 m_rendererTransform;
};

inline std::shared_ptr<Model> Renderer::getModel() {
    return m_model;
}

inline const glm::mat4 & Renderer::getRendererTransform() const {
    return m_rendererTransform;
}

class Sprite;
class ItemizedModel;
struct AnimInfo;



class SpriteRenderer : public Renderer {
public:
	SpriteRenderer(const std::string& anim);
	void setModel(std::shared_ptr<Model>) override;
	std::type_index getType() override;
	void start() override;
	void setAnimation(const std::string&);
	const std::string& getAnimation() const;
	Sprite* getSprite();
	int getFrame() const;
private:
	void innerDraw(Shader*, const glm::mat4&) override;
	const AnimInfo* m_animInfo;
	std::string m_animation;
	int m_frame;
	std::shared_ptr<Sprite> m_sprite;
	int m_ticks;
};

inline Sprite* SpriteRenderer::getSprite() {
    return m_sprite.get();
}

class AnimatedRenderer : public Renderer {
public:
	AnimatedRenderer() = default;
	void setModel(std::shared_ptr<Model>) override;
	std::type_index getType() override;
	void start() override;
private:
	void innerDraw(Shader*, const glm::mat4&) override;
	SpriteRenderer* m_referenceRenderer;
	std::shared_ptr<ItemizedModel> m_itemizedModel;
};
