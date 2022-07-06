#pragma once

#include <unordered_map>
#include <typeindex>
#include <functional>
#include <memory>
#include "model.h"
#include "shape.h"

class ModelMaker {
public:
	ModelMaker();

	std::shared_ptr<Model> get(std::shared_ptr<Shape>);
private:
	std::shared_ptr<Model> makeConvexPoly(std::shared_ptr<Shape> s);
	std::unordered_map<std::type_index, std::function<std::shared_ptr<Model>(std::shared_ptr<Shape>)>> m_builders;
};
