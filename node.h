#pragma once

#include "model.h"
#include "camera.h"


class Node {
public:
	Node();
	~Node();
	void add(std::shared_ptr<Node> node);
	void setModel(std::shared_ptr<Model> model);
	void setPosition(float x, float y, float z);
	int getChildrenCount() const;
	const std::vector<std::shared_ptr<Node>>& children() const;
	void update() {}
	void draw(Shader*);
	void pop();
	std::shared_ptr<Camera> getCamera();
	void setCamera(std::shared_ptr<Camera>);
	const glm::mat4& getModelMatrix() const;
private:
	glm::mat4 m_modelMatrix;
	std::shared_ptr<Camera> m_camera;
	std::shared_ptr<Model> m_model;
	std::vector<std::shared_ptr<Node>> m_children;
};

inline const glm::mat4 & Node::getModelMatrix() const {
	return m_modelMatrix;
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


