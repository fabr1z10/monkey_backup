#include "skeletalmodel.h"
#include <glm/gtx/transform.hpp>
#include <iostream>
#include "../pyhelper.h"
#include "../asset_manager.h"
#include "skeletalrenderer.h"

SkeletalModel::SkeletalModel(const pybind11::kwargs& args) {
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
        JointTransform tr;
        tr.scale = glm::vec3(scale);
        tr.translation = glm::vec3(attachPoint, 0.0f);
        m_jointInfos[id].scale = scale;
        m_jointInfos[id].z = z;
        m_jointInfos[id].restTransform = tr;

        auto mesh = AssetManager::instance().getPolymesh(meshId);
        m_models.push_back(mesh);
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
        for (const auto &j : anims) {
            auto id = j.first.cast<std::string>();
            auto path = j.second.cast<std::string>();
            auto anim = AssetManager::instance().getSkeletalAnimation(path);
            m_animations[id] = anim;
            if (m_defaultAnimation.empty()) {
                m_defaultAnimation = id;
            }
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