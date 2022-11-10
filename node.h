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
	void move(glm::vec3 delta);
	void clearChildren();
	std::vector<std::shared_ptr<Node>> getChildren();
	int getChildrenCount() const;
	void setActive(bool);
	bool active() const;
    const std::unordered_map<long, std::shared_ptr<Node>>& children() const;
    void start();
	void removeChild(long);
	void setFlipX(bool);
	bool getFilpX() const;
	void update(double) ;
	float getScale() const;
	void setScale(float);
	//void setScale(glm::vec3 scaleVector);
	void pop();
	bool getFlipX() const;
	std::shared_ptr<Camera> getCamera();
	void setCamera(std::shared_ptr<Camera>);
    float getMoveTime() const;
	glm::vec3 getLocalPosition() const;
	glm::vec3 getWorldPosition() const;
	const glm::mat4& getModelMatrix() const;
	const glm::mat4 getWorldMatrix() const;
	void setModelMatrix(glm::mat4);
	void setTransformation(float angle, glm::vec3 axis, glm::vec3 pos);
	void move(glm::mat4);

	pybind11::tuple getPos() const;
	//template <typename C>
	void addComponent(std::shared_ptr<Component> c) ;
    float getX() const;
    float getY() const;
	template <typename T>
	T* getComponent() const {
		auto it = m_components.find(std::type_index(typeid(T)));
		if (it != m_components.end()) {
			return dynamic_cast<T*>(it->second.get());
		}
		return nullptr;
	}
    std::string getState() const;
	void setState (const std::string& state, const pybind11::kwargs&);
	void setAnimation(const std::string& animId);
	std::string getAnimation() const;
    void setText(const std::string& text);
	Event<Node*> onMove;						// fires when this node moves
    Event<Node*> onRemove;                      // fires when node is deleted
	void setParent(Node*);
	void setMultColor(glm::vec4 color);
	void setAddColor(glm::vec4 color);
	Node* getParent() ;
	pybind11::object getUserData();
	template<typename T>
	T getValue(const std::string& key) {
		return m_userData[key.c_str()].cast<T>();
	}

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
	glm::mat4 m_scaleMatrix;
	bool m_active;
	pybind11::object m_userData;
	long _id;



};

inline const glm::mat4 & Node::getModelMatrix() const {
	return m_modelMatrix;
}







inline Node * Node::getParent() {
    return m_parent;
}

inline int Node::getChildrenCount() const {
	return m_children.size();
}

inline void Node::setActive(bool active) {
	m_active = active;
}

inline bool Node::active() const {
	return m_active;
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

