#include "modelmake.h"
#include <iostream>
#include "shapes/convexpoly.h"
#include "shapes/circle.h"
#include "models/rawmodel.h"
#include "shapes/compound.h"
#include "pyhelper.h"
#include "models/multi.h"
#include <cmath>

ModelMaker::ModelMaker() {
	m_builders[std::type_index(typeid(Rect))] = [&] (std::shared_ptr<Shape> s, const pybind11::kwargs& args) { return makeConvexPoly(s, args); };
    m_builders[std::type_index(typeid(Circle))] = [&] (std::shared_ptr<Shape> s, const pybind11::kwargs& args) { return makeCircle(s, args); };
    m_builders[std::type_index(typeid(ConvexPoly))] = [&] (std::shared_ptr<Shape> s, const pybind11::kwargs& args) { return makeConvexPoly(s, args); };
    m_builders[std::type_index(typeid(CompoundShape))] = [&] (std::shared_ptr<Shape> s, const pybind11::kwargs& args) { return makeCompoundShape(s, args); };

}

std::shared_ptr<Model> ModelMaker::makeCompoundShape(std::shared_ptr<Shape> s, const pybind11::kwargs &args) {
    auto* cs = static_cast<CompoundShape*>(s.get());
    auto model = std::make_shared<MultiModel>();
    for (const auto& shape : cs->getShapes()) {
        model->addModel(this->get(shape, args));
    }
    return model;
}

std::shared_ptr<Model> ModelMaker::makeConvexPoly(std::shared_ptr<Shape> s, const pybind11::kwargs& args) {
	auto* cp = static_cast<ConvexPoly*>(s.get());
	auto color = dictget<glm::vec4>(args, "color", glm::vec4(1.0f));
	std::vector<float> vertices;
	std::vector<unsigned> elements;
	unsigned u{0};
	for (const auto& p : cp->getPoints()) {
		vertices.insert(vertices.end(), {p.x, p.y, 0.0f, color.r, color.g, color.b, color.a});
		elements.push_back(u++);
	}
	//elements.push_back(0);
	return std::make_shared<RawModel>(ShaderType::SHADER_COLOR, GL_LINE_LOOP, vertices, elements);

}

std::shared_ptr<Model> ModelMaker::makeCircle(std::shared_ptr<Shape> s, const pybind11::kwargs& args) {
    auto* c = static_cast<Circle*>(s.get());
    auto color = dictget<glm::vec4>(args, "color", glm::vec4(1.0f));
    auto n = dictget<int>(args, "points", 20);
    std::vector<float> vertices;
    std::vector<unsigned> elements;
    unsigned u{0};
    auto center = c->getOffset();
    auto radius = c->getRadius();

    float delta = 2.0f * M_PI / n;
    float angle = 0.0f;
    for (int i = 0; i < n; ++i) {
        vertices.insert(vertices.end(), {center.x + radius * cos(angle), center.y + radius * sin(angle), 0.0f, color.r, color.g, color.b, color.a});
        angle += delta;
        elements.push_back(u++);
    }
    return std::make_shared<RawModel>(ShaderType::SHADER_COLOR, GL_LINE_LOOP, vertices, elements);

}


std::shared_ptr<Model> ModelMaker::get(std::shared_ptr<Shape> shape, const pybind11::kwargs& args) {

	auto it = m_builders.find(shape->get_type_index());
	if (it != m_builders.end()) {
		return it->second(shape, args);
	}
	std::cout << " --- don't know how to build model for shape: " << shape->get_type_index().name() << "\n";
	return nullptr;

}