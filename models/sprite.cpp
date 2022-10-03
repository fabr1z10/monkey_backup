#include "sprite.h"
#include "../asset_manager.h"
#include <iostream>
#include "../engine.h"

#include "../symbols.h"
#include "../components/renderer.h"
#include "../pyhelper.h"
#include "../shapes/aabb.h"
#include "../shapes/compound.h"
#include "../modelmake.h"
#include "rawmodel.h"
#include "../node.h"

//
//void ItemizedModel::draw(Shader * s, const glm::mat4& model) {
//	// common stuff, get the items and the transformation
//	std::stringstream key;
//	key << node->getValue<std::string>("anim") << "_" << node->getValue<int>("frame");
//
//
//
//
//	innerDraw(s, node, key.str());
//
//}


void Sprite::init(Node * n) {
	auto obj = n->getUserData();
	obj["anim"] = m_defaultAnimation;
	obj["frame"] = 0;
}

Sprite::Sprite(ShaderType type, GLenum primitive) : Model(type) {
    m_primitive = primitive;
}

Sprite::Sprite(const YAML::Node& node, const std::string& sheetFile) : Model(), m_defaultAnimation(std::string()) {

    m_shaderType = ShaderType::SHADER_TEXTURE;
    m_primitive = GL_TRIANGLES;
    auto& am = AssetManager::instance();
    auto tex = am.getTex(sheetFile);
    float texw = tex->getWidth();
    float texh = tex->getHeight();
    m_texId = tex->getTexId();

	auto& engine = Engine::instance();
	auto defaultTicks = pyget<int>(engine.getConfig().attr("settings"),"ticks", 5);

	// read collision boxes
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


    std::vector<float> vertices;
    std::vector<unsigned> indices;
    float ppu{1.f};
    int quadCount {0};
	for (YAML::const_iterator anit = node["animations"].begin(); anit != node["animations"].end(); ++anit) {
		auto animId = anit->first.as<std::string>();
		if (m_defaultAnimation.empty()) m_defaultAnimation = animId;
		std::cerr << " reading animation:" << animId << "\n";
		AnimInfo animInfo;
		std::vector<FrameInfo> frameInfos;
		animInfo.loop = anit->second["loop"].as<bool>(true);
		int boxAnim = anit->second["box"].as<int>(-1);
		animInfo.frameCount = 0;
		int frameCount = 0;
		for (const auto& el : anit->second["frames"]) {
			FrameInfo frameInfo;
			frameInfo.offset = indices.size();
            frameInfo.count = 0;

			frameInfo.ticks = el["ticks"].as<int>(defaultTicks);
			int boxFrame = el["box"].as<int>(boxAnim);
			frameInfo.box = boxFrame;
			m_frameToShape[std::make_pair(animId, frameCount)] = boxFrame;
			auto ciao = el["quads"].as<std::vector<int>>();
            assert(ciao.size() % 6 == 0);
            int nquads = ciao.size() / 6;
            for (size_t i = 0; i < ciao.size(); i += 6) {
                int width_px = ciao[i+2];
				int height_px = ciao[i+3];
				float tx = ciao[0] / texw;
				float ty = ciao[1] / texh;
				float tw = width_px / texw;
				float th = height_px / texh;
				float ox = ciao[4];
				float oy = ciao[5];
				float width_actual = static_cast<float>(width_px) / ppu;
				float height_actual = static_cast<float>(height_px) / ppu;
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
				frameInfo.count += 6;
				quadCount++;
            }
			    //auto itemId = item["id"].as<std::string>();
				//m_stateToItems[animId + "_" + std::to_string(frameCount)].push_back(m_sheet->getId(itemId));

			frameCount++;
			animInfo.frameCount++;
			animInfo.frameInfo.push_back(frameInfo);
		}
		m_animInfo.insert(std::make_pair(animId, animInfo));
	}
    generateBuffers(vertices, indices);
//		if (m_defaultAnimation.empty()) m_defaultAnimation = anit->first.as<std::string>();
//		std::vector<FrameInfo> frameInfos;
//		AnimInfo animInfo;
//		animInfo.loop = anit->second["loop"].as<bool>(true);
//		animInfo.frames = 0;
//		int boxAnim = anit->second["box"].as<int>(-1);
//		int nframe = 0;
//		for (const auto& el : anit->second["frames""]) {
//			FrameInfo fi;
//			fi.offset = indices.size();
//			fi.count = 0;
//			fi.ticks = el["ticks"].as<int>(1);//el.dictget<int>(el, "ticks", 1);
//			int boxFrame = el["frame"].as<int>(boxAnim);
//			if (boxFrame != -1) m_frameToShape[std::make_pair(anit->first.as<std::string>(), nframe)] = boxFrame;
//			nframe++;
//			animInfo.frames++;
//			for (const auto& quad : el["quads"]) {
//				auto n = quad.size();
//				int width_px = quad[2].as<int>();						// quad width in pixels
//				int height_px = quad[3].as<int>();						// quad height in pixels
//				float tx = quad[0].as<float>() / texw;
//				float ty = quad[1].as<float>() / texh;
//				float tw = static_cast<float>(width_px) / texw;
//				float th = static_cast<float>(height_px) / texh;
//				float ox = (n > 4) ? -quad[4].as<float>() : 0.f;
//				float oy = (n > 5) ? -quad[5].as<float>() : 0.f;
//				float width_actual = static_cast<float>(width_px) / ppu;
//				float height_actual = static_cast<float>(height_px) / ppu;
//				// TODO add flip
//				// bottom left
//				vertices.insert(vertices.end(), {ox, oy, 0.0f, tx, ty + th, 1, 1, 1, 1});
//				// bottom right
//				vertices.insert(vertices.end(), {ox + width_actual, oy, 0.0f, tx + tw, ty + th, 1, 1, 1, 1});
//				// top right
//				vertices.insert(vertices.end(), {ox + width_actual, oy + height_actual, 0.0f, tx + tw, ty, 1, 1, 1, 1});
//				// top left
//				vertices.insert(vertices.end(), {ox, oy + height_actual, 0.0f, tx, ty, 1, 1, 1, 1});
//				unsigned ix = quadCount * 4;
//				indices.insert(indices.end(), {ix, ix + 1, ix + 2, ix + 3, ix, ix + 2});
//				quadCount++;
//				fi.count += 6;
//			}
//			frameInfos.push_back(fi);
//				//frit->second["quads"];
//			//for (auto quit = frit->second["quads"].begin(); quit != frit->second["quads"].end(); ++quit) {
//			//	//quit->second.as<std::vector<float>>();
//			//}
//		}
//		m_frameInfo[anit->first.as<std::string>()] = frameInfos;
//		m_animInfo[anit->first.as<std::string>()] = animInfo;
//	}
//
//	// generate buffers
//	glGenBuffers(1, &m_vbo);
//	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
//
//	glGenBuffers(1, &m_ebo);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
//
//	m_size = indices.size();

}

//void Sprite::draw(Shader* s, Node* node) {
//
//
//
//
//	if (m_texId != GL_INVALID_VALUE) {
//		s->setInt("texture_diffuse1", 0);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, m_texId);
//	}
//
//	Model::draw(s, node);
//
//
//
//}


//
//void Sprite::innerDraw(Shader * s, const glm::mat4& m, const std::string &key) {
//	auto it = m_stateToItems.find(key);
//	if (it == m_stateToItems.end()) {
//		// nothing to draw
//	}
//
//
//	for (size_t i = 0; i < it->second.size(); ++i) {
//		s->setMat4("model", m);
//		auto id = it->second[i];
//		auto it2 = m_transforms.find(std::make_pair(key, i));
//
//
//		// draw current item
//		m_sheet->draw(s, id);
//
//	}
//}

const FrameInfo & Sprite::getFrameInfo(const std::string &anim, int frame) {
	return m_animInfo.at(anim).frameInfo[frame];
}

std::shared_ptr<Renderer> Sprite::getRenderer() const {
	return std::make_shared<SpriteRenderer>(m_defaultAnimation);

}



bool Sprite::hasCollision(const std::string & anim) const {
    return m_frameToShape.count(std::make_pair(anim, 0)) > 0;

}

std::shared_ptr<Shape> Sprite::getShape (const std::string& anim, int frame) const {
    auto it = m_frameToShape.find(std::make_pair(anim, frame));
    if (it == m_frameToShape.end())
        return nullptr;
    if (it->second == -1)
        return nullptr;
    return m_shapes[it->second];
}

std::shared_ptr<Shape> Sprite::getShapeCast (const std::string& anim, int frame) const {
    return nullptr;
}


std::shared_ptr<Model> Sprite::generateDebugModel() {
    std::vector<float> vertices;
    std::vector<unsigned> elements;
    unsigned u{0};
    glm::vec4 color(1.f);
    auto model = std::make_shared<Sprite>(ShaderType::SHADER_COLOR, GL_LINES);
//
//    auto model = std::make_shared<AnimatedModel>(ShaderType::SHADER_COLOR, GL_LINES);
//
    // every shape is associated an offset and a count
    std::unordered_map<int, std::pair<int, int>> m_shapeInfo;
    auto lambda = [&] (const Bounds& b) {
        vertices.insert(vertices.end(), {b.min.x, b.min.y, 0.0f, color.r, color.g, color.b, color.a});
        vertices.insert(vertices.end(), {b.max.x, b.min.y, 0.0f, color.r, color.g, color.b, color.a});
        vertices.insert(vertices.end(), {b.max.x, b.max.y, 0.0f, color.r, color.g, color.b, color.a});
        vertices.insert(vertices.end(), {b.min.x, b.max.y, 0.0f, color.r, color.g, color.b, color.a});
        elements.insert(elements.end(), {u, u+1, u+1, u+2, u+2, u+3, u+3, u});
        u += 4;
    };

    int ishape{0};
    for (const auto& s : m_shapes) {
		unsigned offset = elements.size();
        if (s->getShapeType() == ShapeType::COMPOUND) {
            auto* cs = static_cast<CompoundShape*>(s.get());
            for (const auto& t : cs->getShapes()) {
                lambda(t->getBounds());
            }
        } else {
            lambda(s->getBounds());
        }
        m_shapeInfo[ishape++] = std::make_pair(offset, elements.size() - offset);
//		model->addItem(offset, elements.size() - offset);
    }
	model->generateBuffers(vertices, elements);

    // make a copy of the anim info
    std::unordered_map<std::string, AnimInfo> animInfo(m_animInfo);
    for (auto& a : animInfo) {
        for (auto& f : a.second.frameInfo) {
            if (f.box == -1) {
                f.count = 0;
                f.offset = 0;
            } else {
                auto& p = m_shapeInfo[f.box];
                f.offset = p.first;
                f.count = p.second;
            }
        }
    }
    model->m_animInfo = animInfo;
    return model;
    //return std::make_shared<RawModel>(ShaderType::SHADER_COLOR, GL_LINES, vertices, elements);

}