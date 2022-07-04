#pragma once

#include <vector>
#include <memory>
#include "node.h"

class Room {
public:
	Room(const std::string& id);
	~Room();
	void update();
	void draw(Shader*);
	std::string id() const;
	std::shared_ptr<Node> getRoot();
	void iterate_dfs(std::function<void(Node*)> f);
private:

	std::string m_id;
	std::shared_ptr<Node> m_root;


};


inline std::string Room::id() const {
	return m_id;
}