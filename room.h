#pragma once

#include <vector>
#include <memory>
#include "node.h"
#include "runner.h"

class Room {
public:
	Room(const std::string& id);
	~Room();
	void update();
	void draw(Shader*);
	std::string id() const;
	std::shared_ptr<Node> getRoot();
	void iterate_dfs(std::function<void(Node*)> f);
	void addRunner(std::shared_ptr<Runner> c) {
		m_runners[c->getType()] = c;
	}

	template <typename T>
	T* getRunner() {
		auto it = m_runners.find(std::type_index(typeid(T)));
		if (it != m_runners.end()) {
			return dynamic_cast<T*>(it->second.get());
		}
		return nullptr;
	}
private:

	std::string m_id;
	std::shared_ptr<Node> m_root;
	std::unordered_map<std::type_index, std::shared_ptr<Runner> > m_runners;


};


inline std::string Room::id() const {
	return m_id;
}