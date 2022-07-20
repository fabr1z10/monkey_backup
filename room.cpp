#include <iostream>
#include "room.h"
#include "components/renderer.h"
#include <glm/glm.hpp>

Room::Room(const std::string& id) : m_id(id) {
	m_root = std::make_shared<Node>();

}
Room::~Room() {
	std::cout << "destroy room\n";
}

Node::Node() : /*m_model(nullptr),*/ m_camera(nullptr), m_modelMatrix(1.0f), m_active(true), m_parent(nullptr), m_worldMatrix(1.0f) {}



void Node::setPosition(float x, float y, float z) {
	m_modelMatrix[3][0] = x;
	m_modelMatrix[3][1] = y;
	m_modelMatrix[3][2] = z;
}

Node::~Node() {
	std::cout << "destroy node\n";
}

//void Node::draw(Shader* s) {
//	if (m_model != nullptr) {
//		m_model->draw(s);
//	}
//	//std::cout << "drawing " << this << "\n";
//}

void Node::pop() {
	std::cout << " popping...\n";
}
std::shared_ptr<Node> Room::getRoot() {
	return m_root;
}

void Node::add(std::shared_ptr<Node> node) {
	m_children.push_back(node);
	node->setParent(this);

	// TODO call start if engine is running (node added on the fly)

}

void Room::iterate_dfs(std::function<void(Node*)> f) {
	std::vector<Node*> li;
	li.push_back(m_root.get());
	while (!li.empty()) {
		auto current = li.back();
		li.pop_back();
		f(current);
		for (const auto& child : current->children()) {
			li.push_back(child.get());
		}
	}
}

void Room::update(double dt) {
	std::vector<Node*> li;
	li.push_back(m_root.get());
	while (!li.empty()) {
		auto current = li.back();
		li.pop_back();
		current->update(dt);
		// update world transform
		for (const auto& child : current->children()) {
			li.push_back(child.get());
		}
	}
	for (const auto& r : m_runners) {
		r.second->update(dt);
	}
}

void Room::draw(Shader* s) {
	typedef void(Node::*BarkFunction)(void);

	std::vector<std::pair<int, glm::mat4>> m_matrices;
	std::vector<std::pair<int, std::shared_ptr<Camera>>> camStack;
	std::vector<Node*> li;
	li.push_back(m_root.get());
	glm::mat4 viewMatrix(1.0f);
	while (!li.empty()) {
		auto current = li.back();
		int currentIndex = li.size() - 1;

		// check if current node has a camera
		auto cam = current->getCamera();
		bool changeCam = false;
		while (!camStack.empty() && currentIndex < camStack.back().first) {
			// need to pop cam from stack
			changeCam = true;
			camStack.pop_back();
		}
		if (cam != nullptr) {
			changeCam = true;
			camStack.emplace_back(currentIndex, cam);
		}
		if (changeCam && !camStack.empty()) {
			camStack.back().second->init(s);
			viewMatrix = camStack.back().second->getViewMatrix();
		}


		while (!m_matrices.empty() && currentIndex < m_matrices.back().first) {
			// need to pop last matrix from stack
			m_matrices.pop_back();
		}
		li.pop_back();

		// setup modelview
		auto renderer = current->getComponent<Renderer>();
		if (renderer != nullptr) {
			glm::mat4 mvm =  viewMatrix * current->getWorldMatrix() * renderer->getRendererTransform();
			s->setMat4("modelview", mvm);
			renderer->draw(s);
		}
		for (const auto& child : current->children()) {
			li.push_back(child.get());
		}

	}

}