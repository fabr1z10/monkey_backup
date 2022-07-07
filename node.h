#pragma once

#include "model.h"
#include "camera.h"
#include "component.h"
#include "event.h"

class Node {
public:
	Node();
	~Node();
	void add(std::shared_ptr<Node> node);
	void setModel(std::shared_ptr<Model> model);
	void setPosition(float x, float y, float z);
	int getChildrenCount() const;
	const std::vector<std::shared_ptr<Node>>& children() const;
	void start();
	void update(double) ;
	//void draw(Shader*);
	void pop();
	std::shared_ptr<Camera> getCamera();
	void setCamera(std::shared_ptr<Camera>);
	glm::vec3 getWorldPosition() const;
	const glm::mat4& getModelMatrix() const;
	const glm::mat4& getWorldMatrix() const;
	void setModelMatrix(glm::mat4);
	//template <typename C>
	void addComponent(std::shared_ptr<Component> c) {
		m_components[c->getType()] = c;
		c->setNode(this);
	}

	template <typename T>
	T* getComponent() {
		auto it = m_components.find(std::type_index(typeid(T)));
		if (it != m_components.end()) {
			return dynamic_cast<T*>(it->second.get());
		}
		return nullptr;
	}

	Event<Node*> onMove;						// fires when this node moves

	void setParent(Node*);

private:
	glm::mat4 m_modelMatrix;
	std::shared_ptr<Camera> m_camera;
	//std::shared_ptr<Model> m_model;
	std::vector<std::shared_ptr<Node>> m_children;
	Node* m_parent;
	std::unordered_map<std::type_index, std::shared_ptr<Component> > m_components;
	glm::mat4 m_worldMatrix;
	bool m_active;
};

inline const glm::mat4 & Node::getModelMatrix() const {
	return m_modelMatrix;
}

inline void Node::setModelMatrix(glm::mat4 m) {
	m_modelMatrix = m;
}

inline const glm::mat4 & Node::getWorldMatrix() const {
	return m_worldMatrix;
}


inline int Node::getChildrenCount() const {
	return m_children.size();
}

inline const std::vector<std::shared_ptr<Node>> & Node::children() const {
	return m_children;

}

inline std::shared_ptr<Camera> Node::getCamera() {
	return m_camera;
}

inline void Node::setCamera(std::shared_ptr<Camera> cam) {
	m_camera = cam;
}


