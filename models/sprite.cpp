#include "sprite.h"
#include "../asset_manager.h"
#include <iostream>

#include "../symbols.h"
#include "../components/renderer.h"
#include "../pyhelper.h"
#include "../shapes/aabb.h"
#include "../shapes/compound.h"
#include "../modelmake.h"
#include "rawmodel.h"

Sprite::Sprite(const YAML::Node& node) : Model(ShaderType::SHADER_TEXTURE), m_defaultAnimation(std::string()) {

	std::vector<GLfloat> vertices;
	std::vector<unsigned> indices;

	auto sheet = node["sheet"].as<std::string>();
	auto& am = AssetManager::instance();
	auto tex = am.getTex(sheet);
	m_texId = tex->getTexId();
	auto texw = tex->getWidth();
	auto texh = tex->getHeight();
	float ppu = 1.0f;
	int quadCount = 0;
	// read shapes

    for (YAML::const_iterator anit = node["boxes"].begin(); anit != node["boxes"].end(); ++anit) {
        auto a = (*anit).as<std::vector<float>>();
        assert(a.size() % 4 == 0);
        std::shared_ptr<Shape> shape;
        if (a.size() == 4) {
            shape = std::make_shared<AABB>(a[0], a[1], a[2], a[3]);
        } else {
            auto s1 = std::make_shared<CompoundShape>();
            for (size_t i = 0; i < a.size(); i+=4) {
                s1->addShape(std::make_shared<AABB>(a[i], a[i+1], a[i+2], a[i+3]));
            }
            shape = s1;
        }
        m_staticBounds.expandWith(shape->getBounds());
        m_shapes.push_back(shape);
    }



	for (YAML::const_iterator anit = node["animations"].begin(); anit != node["animations"].end(); ++anit) {
		//std::cout << " --- anim: " << anit->first << "\n";
		if (m_defaultAnimation.empty()) m_defaultAnimation = anit->first.as<std::string>();
		std::vector<FrameInfo> frameInfos;
		AnimInfo animInfo;
		animInfo.loop = anit->second["loop"].as<bool>(true);
		animInfo.frames = 0;
		int boxAnim = anit->second["box"].as<int>(-1);
		int nframe = 0;
		for (const auto& el : anit->second["elements"]) {
			FrameInfo fi;
			fi.offset = indices.size();
			fi.count = 0;
			fi.ticks = el["ticks"].as<int>(1);//el.dictget<int>(el, "ticks", 1);
			int boxFrame = el["frame"].as<int>(boxAnim);
			if (boxFrame != -1) m_frameToShape[std::make_pair(anit->first.as<std::string>(), nframe)] = boxFrame;
			nframe++;
			animInfo.frames++;
			for (const auto& quad : el["quads"]) {
				auto n = quad.size();
				int width_px = quad[2].as<int>();						// quad width in pixels
				int height_px = quad[3].as<int>();						// quad height in pixels
				float tx = quad[0].as<float>() / texw;
				float ty = quad[1].as<float>() / texh;
				float tw = static_cast<float>(width_px) / texw;
				float th = static_cast<float>(height_px) / texh;
				float ox = (n > 4) ? -quad[4].as<float>() : 0.f;
				float oy = (n > 5) ? -quad[5].as<float>() : 0.f;
				float width_actual = static_cast<float>(width_px) / ppu;
				float height_actual = static_cast<float>(height_px) / ppu;
				// TODO add flip
				// bottom left
				vertices.insert(vertices.end(), {ox, oy, 0.0f, tx, ty + th, 1, 1, 1, 1});
				// bottom right
				vertices.insert(vertices.end(), {ox + width_actual, oy, 0.0f, tx + tw, ty + th, 1, 1, 1, 1});
				// top right
				vertices.insert(vertices.end(), {ox + width_actual, oy + height_actual, 0.0f, tx + tw, ty, 1, 1, 1, 1});
				// top left
				vertices.insert(vertices.end(), {ox, oy + height_actual, 0.0f, tx, ty, 1, 1, 1, 1});
				unsigned ix = quadCount * 4;
				indices.insert(indices.end(), {ix, ix + 1, ix + 2, ix + 3, ix, ix + 2});
				quadCount++;
				fi.count += 6;
			}
			frameInfos.push_back(fi);
				//frit->second["quads"];
			//for (auto quit = frit->second["quads"].begin(); quit != frit->second["quads"].end(); ++quit) {
			//	//quit->second.as<std::vector<float>>();
			//}
		}
		m_frameInfo[anit->first.as<std::string>()] = frameInfos;
		m_animInfo[anit->first.as<std::string>()] = animInfo;
	}

	// generate buffers
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

	m_size = indices.size();

}

void Sprite::draw(Shader* s, int offset, int size) {

	if (m_texId != GL_INVALID_VALUE) {
		s->setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texId);
	}

	Model::draw(s, offset, size);



}

const FrameInfo & Sprite::getFrameInfo(const std::string &anim, int frame) {
	return m_frameInfo[anim][frame];
}

std::shared_ptr<Renderer> Sprite::getRenderer() const {
	return std::make_shared<SpriteRenderer>(m_defaultAnimation);

}

bool Sprite::hasCollision(const std::string & anim) const {
    return m_frameToShape.count(std::make_pair(anim, 0)) > 0;

}

std::shared_ptr<Shape> Sprite::getShape (const std::string& anim, int frame) const {
    return m_shapes[m_frameToShape.at(std::make_pair(anim, frame))];
}

std::shared_ptr<Shape> Sprite::getShapeCast (const std::string& anim, int frame) const {
    return nullptr;
}


std::shared_ptr<Model> Sprite::generateDebugModel() {
    std::vector<float> vertices;
    std::vector<unsigned> elements;
    unsigned u{0};
    glm::vec4 color(1.f);
    auto lambda = [&] (const Bounds& b) {
        vertices.insert(vertices.end(), {b.min.x, b.min.y, 0.0f, color.r, color.g, color.b, color.a});
        vertices.insert(vertices.end(), {b.max.x, b.min.y, 0.0f, color.r, color.g, color.b, color.a});
        vertices.insert(vertices.end(), {b.max.x, b.max.y, 0.0f, color.r, color.g, color.b, color.a});
        vertices.insert(vertices.end(), {b.min.x, b.max.y, 0.0f, color.r, color.g, color.b, color.a});
        elements.insert(elements.end(), {u, u+1, u+1, u+2, u+2, u+3, u+3, u});
        u += 4;
    };

    for (const auto& s : m_shapes) {
        if (s->getShapeType() == ShapeType::COMPOUND) {
            auto* cs = static_cast<CompoundShape*>(s.get());
            for (const auto& t : cs->getShapes()) {
                lambda(t->getBounds());
            }
        } else {
            lambda(s->getBounds());
        }
    }
    return std::make_shared<RawModel>(ShaderType::SHADER_COLOR, GL_LINES, vertices, elements);

}