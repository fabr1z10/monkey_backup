#include "shapemodel.h"
#include "../models/rawmodel.h"


std::shared_ptr<Model> AABBmodel(AABB* s, glm::vec4 color, FillType fillType) {

	std::vector<float> vertices;
	std::vector<unsigned> elements;
	unsigned u{0};
	auto b = s->getBounds();
	vertices.insert(vertices.end(), {b.min.x, b.min.y, 0.0f, color.r, color.g, color.b, color.a});
	vertices.insert(vertices.end(), {b.max.x, b.min.y, 0.0f, color.r, color.g, color.b, color.a});
	vertices.insert(vertices.end(), {b.max.x, b.max.y, 0.0f, color.r, color.g, color.b, color.a});
	vertices.insert(vertices.end(), {b.min.x, b.max.y, 0.0f, color.r, color.g, color.b, color.a});

	GLuint prim {GL_LINE_LOOP};
	if (fillType == FillType::OUTLINE) {
		elements.insert(elements.end(), {0, 1, 2, 3});
	} else {
		elements.insert(elements.end(), {0, 1, 2, 2, 3, 0});
		prim = GL_TRIANGLES;
	}

	return std::make_shared<RawModel>(ShaderType::SHADER_COLOR, prim, vertices, elements);


}