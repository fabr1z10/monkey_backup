#include "node.h"
#include "components/renderer.h"

void Node::setModel(std::shared_ptr<Model> model) {
	auto renderer = model->getRenderer();
	renderer->setModel(model);
	addComponent(renderer);

	//m_model = model;
}

void Node::start() {
	for (auto& c : m_components){
		c.second->start();
	}
}

void Node::setParent(Node * node) {
	m_parent = node;
}