#pragma once

#include <iostream>
#include "model.h"
#include "camera.h"
#include "component.h"
#include "event.h"


class Node {
public:
	Node();
	~Node();
	long getId() const;
	void add(std::shared_ptr<Node> node);
	void remove();
	void setModel(std::shared_ptr<Model> model);
	void setPosition(float x, float y, float z);
	int getChildrenCount() const;
    const std::unordered_map<long, std::shared_ptr<Node>>& children() const;
    void start();
	void removeChild(long);
	void setFlipX(bool);
	bool getFilpX() const;
	void update(double) ;
	//void draw(Shader*);
	void pop();
	std::shared_ptr<Camera> getCamera();
	void setCamera(std::shared_ptr<Camera>);
	glm::vec3 getWorldPosition() const;
	const glm::mat4& getModelMatrix() const;
	const glm::mat4& getWorldMatrix() const;
	void setModelMatrix(glm::mat4);
	void move(glm::mat4);
	pybind11::tuple getPos() const;
	//template <typename C>
	void addComponent(std::shared_ptr<Component> c) ;

	template <typename T>
	T* getComponent() {
		auto it = m_components.find(std::type_index(typeid(T)));
		if (it != m_components.end()) {
			return dynamic_cast<T*>(it->second.get());
		}
		return nullptr;
	}

	void setAnimation(const std::string& animId);
	Event<Node*> onMove;						// fires when this node moves
    Event<Node*> onRemove;
	void setParent(Node*);
	void setMultColor(float r, float g, float b, float a);
	Node* getParent() ;
	pybind11::object getUserData();
	void setUserData(pybind11::object);
    std::unordered_map<long, std::shared_ptr<Node>> m_children;
private:
    bool m_started;

	glm::mat4 m_modelMatrix;
	std::shared_ptr<Camera> m_camera;
	//std::shared_ptr<Model> m_model;
	Node* m_parent;
	std::unordered_map<std::type_index, std::shared_ptr<Component> > m_components;
	glm::mat4 m_worldMatrix;
	bool m_active;
	pybind11::object m_userData;
	long _id;



};

inline const glm::mat4 & Node::getModelMatrix() const {
	return m_modelMatrix;
}




inline const glm::mat4 & Node::getWorldMatrix() const {
	return m_worldMatrix;
}

inline Node * Node::getParent() {
    return m_parent;
}

inline int Node::getChildrenCount() const {
	return m_children.size();
}

inline const std::unordered_map<long, std::shared_ptr<Node>> & Node::children() const {
	return m_children;

}

inline std::shared_ptr<Camera> Node::getCamera() {
	return m_camera;
}

inline void Node::setCamera(std::shared_ptr<Camera> cam) {
	m_camera = cam;
}

inline long Node::getId() const {
    return _id;
}