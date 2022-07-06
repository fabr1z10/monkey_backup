#include "modelmake.h"
#include <iostream>
#include "shapes/convexpoly.h"
#include "models/rawmodel.h"

ModelMaker::ModelMaker() {
	m_builders[std::type_index(typeid(Rect))] = [&] (std::shared_ptr<Shape> s) { return makeConvexPoly(s); };

}

std::shared_ptr<Model> ModelMaker::makeConvexPoly(std::shared_ptr<Shape> s) {
	auto* cp = static_cast<ConvexPoly*>(s.get());
	std::vector<float> vertices;
	std::vector<unsigned> elements;
	unsigned u{0};
	for (const auto& p : cp->getPoints()) {
		vertices.insert(vertices.end(), {p.x, p.y, 0.0f, 1.0, 1.0, 1.0, 1.0});
		elements.push_back(u++);
	}
	//elements.push_back(0);
	return std::make_shared<RawModel>(ShaderType::SHADER_COLOR, GL_LINE_LOOP, vertices, elements);

}


std::shared_ptr<Model> ModelMaker::get(std::shared_ptr<Shape> shape) {

	auto it = m_builders.find(shape->get_type_index());
	if (it != m_builders.end()) {
		return it->second(shape);
	}
	std::cout << " --- don't know how to build model for shape: " << shape->get_type_index().name() << "\n";
	return nullptr;

}