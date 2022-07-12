#pragma once

#include "../component.h"
#include "../model.h"



class Renderer : public Component {
public:
	Renderer();
	void draw(Shader*);
	virtual void setModel(std::shared_ptr<Model>);
	void setMultColor(glm::vec4);
	void setAddColor(glm::vec4);
protected:
	virtual void innerDraw(Shader*) ;
private:
	std::shared_ptr<Model> m_model;
	glm::vec4 m_multColor;
	glm::vec4 m_addColor;
};


class Sprite;
struct AnimInfo;

class SpriteRenderer : public Renderer {
public:
	SpriteRenderer(const std::string& anim);
	void setModel(std::shared_ptr<Model>) override;
	std::type_index getType() override;
	void start() override;
private:
	void innerDraw(Shader*) override;
	const AnimInfo* m_animInfo;
	std::string m_animation;
	int m_frame;
	std::shared_ptr<Sprite> m_sprite;
	int m_ticks;
};