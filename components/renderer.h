#pragma once

#include "../component.h"
#include "../model.h"


class Renderer : public Component {
public:
	void draw(Shader*);
	void setModel(std::shared_ptr<Model>);
private:

	std::shared_ptr<Model> m_model;


};