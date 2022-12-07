#pragma once

#include <unordered_map>
#include <typeindex>
#include <functional>
#include <memory>
#include "model.h"
#include "shape.h"

class ModelMaker {
public:
	static ModelMaker& instance() {
		static ModelMaker instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}



	std::shared_ptr<Model> get(std::shared_ptr<Shape>, const pybind11::kwargs& args);
private:
	ModelMaker();
	std::shared_ptr<Model> makeConvexPoly(std::shared_ptr<Shape> s, const pybind11::kwargs& args);
    std::shared_ptr<Model> makeCircle(std::shared_ptr<Shape> s, const pybind11::kwargs& args);
    std::shared_ptr<Model> makeCompoundShape(std::shared_ptr<Shape> s, const pybind11::kwargs& args);
    std::shared_ptr<Model> makeAABB(std::shared_ptr<Shape> s, const pybind11::kwargs& args);
	std::shared_ptr<Model> makeAABB3D(std::shared_ptr<Shape> s, const pybind11::kwargs& args);

    std::unordered_map<std::type_index, std::function<std::shared_ptr<Model>(std::shared_ptr<Shape>, const pybind11::kwargs& args)>> m_builders;
};
