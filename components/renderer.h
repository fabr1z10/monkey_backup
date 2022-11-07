#pragma once

#include "../component.h"
#include "../model.h"
#include "../models/sprite.h"


class Renderer : public Component {
public:
	Renderer();
	virtual int setup(Shader*);
	virtual void draw(Shader*);
	std::shared_ptr<Model> getModel();
	virtual void setModel(std::shared_ptr<Model>);
	void setMultColor(glm::vec4);
	void setAddColor(glm::vec4);
	const glm::mat4& getRendererTransform() const;
	void flipHorizontal(bool);
	bool getFlipHorizontal() const;
	void setTransform(const glm::mat4& m);
	void setCount(int);
	void setOffset(int);
//protected:
//	virtual void innerDraw(Shader*, const glm::mat4& modelTransform) ;
protected:
	std::shared_ptr<Model> m_model;
	glm::vec4 m_multColor;
	glm::vec4 m_addColor;
	glm::mat4 m_rendererTransform;
	int m_offset;
	int m_count;

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

class AnimatedRenderer : public Renderer {
public:
    explicit AnimatedRenderer(const std::string& defaultAnimation);
    virtual const std::string& getAnimation() const;
    virtual void setAnimation(const std::string&) = 0;
    bool isComplete() const;
protected:
    bool m_complete;
    // the current animation
    std::string m_animation;
};

inline bool AnimatedRenderer::isComplete() const {
    return m_complete;
}


class SpriteRenderer : public AnimatedRenderer {
public:
	explicit SpriteRenderer(const std::string& anim);
	void setModel(std::shared_ptr<Model>) override;
	std::type_index getType() override;
	void start() override;
    void setAnimation(const std::string&) override;
	Sprite* getSprite();
	virtual int getFrame() const;
	void draw(Shader *) override;
private:
	//void innerDraw(Shader*, const glm::mat4&) override;
	const AnimInfo* m_animInfo;
	//std::string m_animation;
	int m_frame;
	std::shared_ptr<Sprite> m_sprite;
	int m_ticks;
};

class CopySpriteRenderer : public SpriteRenderer {
public:
    CopySpriteRenderer(const std::string& anim) : SpriteRenderer(anim) {}
    void start() override;
    const std::string& getAnimation() const override;
    int getFrame() const override;
private:
    SpriteRenderer* m_reference;
};


inline Sprite* SpriteRenderer::getSprite() {
    return m_sprite.get();
}

//class AnimatedRenderer : public Renderer {
//public:
//	AnimatedRenderer() = default;
//	void setModel(std::shared_ptr<Model>) override;
//	std::type_index getType() override;
//	void start() override;
//private:
//	//void innerDraw(Shader*, const glm::mat4&) override;
//	SpriteRenderer* m_referenceRenderer;
//	std::shared_ptr<ItemizedModel> m_itemizedModel;
//};
