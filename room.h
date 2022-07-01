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
private:

	std::string m_id;
	std::shared_ptr<Node> m_root;


};


inline std::string Room::id() const {
	return m_id;
}