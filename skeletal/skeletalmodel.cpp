#include "skeletalmodel.h"
#include <glm/gtx/transform.hpp>
#include <iostream>
#include "../pyhelper.h"
#include "../asset_manager.h"
#include "skeletalrenderer.h"
#include "../shapes/aabb.h"
#include "../shapes/compound.h"
#include <pybind11/stl.h>


SkeletalModel::SkeletalModel(const pybind11::kwargs& args) : Model(ShaderType::SHADER_SKELETAL) {
    for (const auto& j : args["joints"]) {
        auto dict = j.cast<pybind11::dict>();
        int id = m_jointInfos.size();
        auto jointId = j["id"].cast<std::string>();
        auto meshId = j["mesh"].cast<std::string>();
        auto parent = dictget<std::string>(dict, "parent", "");
        std::string keyPoint;
        int parentId = -1;
        if (!parent.empty()) {
            auto sep = parent.find('@');
            assert(sep != std::string::npos);
            parentId = m_jointNameToId[parent.substr(0, sep)];
            keyPoint = parent.substr(sep+1);
        }
        m_jointInfos.emplace_back(id, parentId, jointId, glm::ivec3(id, parentId == -1 ? 0 : parentId, 0));
        std::cout << "*** joint " << jointId << ", id: " << id << " , " << meshId << ", parent = " << parent << "\n";
        glm::vec2 attachPoint(0.f);
        if (parentId != -1) {
            m_jointInfos[parentId].children.push_back(id);
            attachPoint = m_models[parentId]->getKeyPoint(keyPoint);

        } else {
            m_root = id;
        }
        auto scale = dictget<float>(dict, "scale", 1.f);
        auto z = dictget<float>(dict, "z", 0.f);
        auto offset = dictget<glm::vec2>(dict, "offset", glm::vec2(0.f));
        attachPoint += offset;
        JointTransform tr;
        tr.scale = glm::vec3(scale);
        tr.translation = glm::vec3(attachPoint, 0.0f);
        m_jointInfos[id].scale = scale;
        m_jointInfos[id].z = z;
        m_jointInfos[id].restTransform = tr;

        auto mesh = AssetManager::instance().getPolymesh(meshId);
        m_models.push_back(mesh);
        m_jointInfos[id].mesh = mesh.get();
        m_jointNameToId.insert(std::make_pair(jointId, id));
    }

    std::unordered_map<int, JointTransform> p;
    m_invRestTransforms2 = std::vector<glm::mat4>(m_jointInfos.size());
    for (size_t i = 0; i < m_invRestTransforms2.size(); ++i) {
        m_invRestTransforms2[i] = glm::mat4(1.0f);
    }
    m_restTransforms2 = calculateCurrentPose(p);
    for (size_t i = 0; i < m_invRestTransforms2.size(); ++i) {
        m_restTransforms2[i][3][2] = m_jointInfos[i].z;
        m_invRestTransforms2[i] = glm::inverse(m_restTransforms2[i]);
        //m_invRestTransforms2[i][3][2] = -m_jointInfos[i].z;
    }

    int ac = 0;
    if (args.contains("animations")) {
        auto anims = args["animations"].cast<pybind11::dict>();
        for (const auto& j : anims) {
            auto id = j.first.cast<std::string>();
            auto animInfo = j.second.cast<pybind11::dict>();
            auto path = dictget<std::string>(animInfo, "id", "");//animInfo["id"].cast<std::string>();
            auto box = dictget(animInfo, "box", -1);
            std::vector<int> shapeVector;
            std::vector<int> attackBox;
            //if (box != -1) {
            shapeVector.push_back(box);
            //}
            if (!path.empty()) {
                auto anim = AssetManager::instance().getSkeletalAnimation(path);
                attackBox = dictget<std::vector<int>>(animInfo, "attack_box", std::vector<int>());
                m_animations[id] = anim;
            }
            shapeVector.insert(shapeVector.end(), attackBox.begin(), attackBox.end());
            m_animShapes[id] = shapeVector;
            if (m_defaultAnimation.empty()) {
                m_defaultAnimation = id;
            }

        }
    }

    // ##################
    // read offset
    // ##################
    if (args.contains("offset")) {
        for (const auto& offset : args["offset"]) {
            auto id = offset.cast<std::string>() ;
            auto atindex = id.find('@');
            assert(atindex != std::string::npos);
            m_offsetPointIds.emplace_back(id.substr(0, atindex), id.substr(atindex+1));
        }

//        auto ids = args["offset"].cast<std::vector<std::string>>();
//        for (const auto& id : ids) {
//        }
    }
    computeOffset();

    // read collision boxes
    if (args.contains("boxes")) {
        for (const auto& b : args["boxes"]) {
            auto f = b.cast<std::vector<float>>();
            assert(f.size() % 4 == 0);
            std::shared_ptr<Shape > shape;
            if (f.size() == 4) {
                shape = std::make_shared<AABB>(f[0], f[1], f[2], f[3]);
            } else {
                auto s1 = std::make_shared<CompoundShape>();
                for (size_t i = 0; i <f.size(); i+=4) {
                    s1->addShape(std::make_shared<AABB>(f[i], f[i+1], f[i+2], f[i+3]));
                }
                shape = s1;
            }
            m_staticBounds.expandWith(shape->getBounds());
            m_shapes.push_back(shape);
        }
    }


//    main.foreach("animations", [&] (const std::string& id, const ITab& node) {
//
//        auto animId = node.as<std::string>();
//        if (ac == 0) {
//            m_defaultAnimation = id;
//        }
//        setAnimation(id, animId);
//        ac++;
//    });

}

std::shared_ptr<Renderer> SkeletalModel::getRenderer() const {
    return std::make_shared<SkeletalRenderer>(m_defaultAnimation);
}



std::vector<glm::mat4> SkeletalModel::calculateCurrentPose(std::unordered_map<int, JointTransform>& pose) {
    std::vector<glm::mat4> result(m_jointInfos.size());
    std::list<std::pair<int, glm::mat4>> joints;
    joints.emplace_back(m_root, glm::mat4(1.0));
    while (!joints.empty()) {
        auto current = joints.front();
        joints.pop_front();
        const auto& joint = m_jointInfos[current.first];
        auto localTransform = joint.restTransform;
        // TODO apply local transform
        auto i = pose.find(current.first);
        if (i != pose.end()) {
            localTransform += i->second;
        }
        auto localMat = localTransform.getLocalTransform();
        auto modelMat = current.second * localMat * glm::scale(glm::vec3(joint.scale));
        result[current.first] = modelMat * m_invRestTransforms2[current.first];
        // TODO convert to global mat
        for (const auto &child : joint.children) {
            joints.emplace_back(child, modelMat);
        }
    }
    return result;

}

const glm::mat4& SkeletalModel::getRestTransform(int id) const {
    return m_restTransforms2[id];
}

glm::ivec3 SkeletalModel::getWeightIndex(int id) const {
    return m_jointInfos[id].weightIndex;
}

SkeletalAnimation * SkeletalModel::getAnimation(const std::string &id) {
    auto it = m_animations.find(id);
    if (it == m_animations.end())
        return nullptr;
    return it->second.get();
}

void SkeletalModel::computeOffset() {
    m_offsetPoints.clear();
    for (const auto& p : m_offsetPointIds) {
        int jointId = m_jointNameToId.at(p.first);
        if (m_jointInfos[jointId].mesh == nullptr) {
            continue;
        }
        auto kp = m_jointInfos[jointId].mesh->getKeyPoint(p.second);
        auto mp = m_restTransforms2[jointId] * glm::vec4(kp.x, kp.y, 0.0f, 1.0f);
        m_offsetPoints.emplace_back(jointId, glm::vec3(mp.x, mp.y, 0.0f));
    }
}

std::shared_ptr<Shape> SkeletalModel::getShape (const std::string& anim) const {
    const auto& it = m_animShapes.find(anim);
    if (it == m_animShapes.end())
        return nullptr;
    return m_shapes[it->second[0]];
}



bool SkeletalModel::hasCollision(const std::string & anim) const {
    return m_animShapes.count(anim) > 0;

}

std::shared_ptr<Model> SkeletalModel::generateDebugModel() {
    std::vector<float> vertices;
    std::vector<unsigned> elements;
    unsigned u{0};
    glm::vec4 color(1.f);
    auto model = std::make_shared<Model>(ShaderType::SHADER_COLOR, GL_LINES);

    // every shape is associated an offset and a count
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
        m_shapeInfo.emplace_back(offset, elements.size() - offset);
    }
    model->generateBuffers(vertices, elements);

    return model;
    //return std::make_shared<RawModel>(ShaderType::SHADER_COLOR, GL_LINES, vertices, elements);

}

std::pair<int, int> SkeletalModel::getDebugShape(const std::string &anim, int n) {
    const auto& it = m_animShapes.find(anim);
    if (it == m_animShapes.end() || n >= it->second.size())
        return std::make_pair(-1, -1);

    auto shapeId = it->second[n];
    return m_shapeInfo[shapeId];
}