#pragma once

#include "../collision.h"
#include "../shape.h"
#include <unordered_map>
#include <functional>
#include <utility>
#include "../hashpair.h"

class Intersector {
public:
	Intersector() {}
	CollisionReport intersect(Shape*, Shape*, const glm::mat4&, const glm::mat4&) ;

	template<typename T, typename S>
	void add(std::function<CollisionReport(Shape*, Shape*, const glm::mat4&, const glm::mat4&)> f) {
		m_functionMap[std::make_pair(std::type_index(typeid(T)), std::type_index(typeid(S)))] = std::move(f);
	}


protected:
	std::unordered_map<std::pair<std::type_index, std::type_index>, std::function<CollisionReport(Shape*, Shape*, const glm::mat4&, const glm::mat4&)>> m_functionMap;
};


class Intersector2D : public Intersector {
public:
	Intersector2D();

private:
	CollisionReport performSAT(const std::vector<glm::vec2>& axesw, const Shape2D* a, const Shape2D* b, const glm::mat4& t1, const glm::mat4& t2);

	CollisionReport SAT(Shape*, Shape*, const glm::mat4&, const glm::mat4&);


};