#include "node.h"
#include "components/renderer.h"

void Node::setModel(std::shared_ptr<Model> model) {
	auto renderer = model->getRenderer();
	renderer->setModel(model);
	addComponent(renderer);

	//m_model = model;
}

void Node::start() {
	// update world matrix
	if (m_parent != nullptr) {
		m_worldMatrix = m_parent->getWorldMatrix() * m_modelMatrix;
	}
	for (auto& c : m_components){
		c.second->start();
	}
}

void Node::move(glm::mat4 m) {
	m_modelMatrix *= m;
	onMove.fire(this);
}

void Node::setFlipX(bool value) {
	m_modelMatrix[0] = glm::vec4(value ? -1.0f : 1.0f, 0.f, 0.f, 0.f);
}

void Node::update(double dt) {
	if (!m_active) return;
	for (auto& iter : m_components) {
		iter.second->update(dt);
	}
	// update world matrix
	if (m_parent != nullptr) {
		m_worldMatrix = m_parent->getWorldMatrix() * m_modelMatrix;
	}
}

void Node::setParent(Node * node) {
	m_parent = node;
}

glm::vec3 Node::getWorldPosition() const {
	return glm::vec3(m_worldMatrix[3]);
}

pybind11::tuple Node::getPos() const {
	return pybind11::make_tuple(m_worldMatrix[3][0], m_worldMatrix[3][1], m_worldMatrix[3][2]);

}


void Node::setMultColor(float r, float g, float b, float a) {
	getComponent<Renderer>()->setMultColor(glm::vec4(r,g,b,a));
}