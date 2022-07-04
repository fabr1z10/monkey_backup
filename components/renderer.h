#pragma once

#include "../component.h"
#include "../model.h"



class Renderer : public Component {
public:
	virtual void draw(Shader*);
	virtual void setModel(std::shared_ptr<Model>);
private:

	std::shared_ptr<Model> m_model;


};


class Sprite;
struct AnimInfo;

class SpriteRenderer : public Renderer {
public:
	SpriteRenderer(const std::string& anim);
	void draw(Shader*) override;
	void setModel(std::shared_ptr<Model>) override;
	std::type_index getType() override;
	void start() override;
private:
	const AnimInfo* m_animInfo;
	std::string m_animation;
	int m_frame;
	std::shared_ptr<Sprite> m_sprite;
	int m_ticks;
};