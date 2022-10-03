#pragma once

#include "../model.h"
#include "../jointtransform.h"
#include "../models/polymesh.h"
#include "skeletalanimation.h"
#include <pybind11/pybind11.h>

struct JointInfo {
    JointInfo(int id, int parent, const std::string& name, glm::ivec3 weightIndex) : id(id),
        parent(parent), name(name), mesh(nullptr), weightIndex(weightIndex), scale(1.0f), z(0.0f) {}
    int id;
    int parent;
    float scale;
    float z;
    std::vector<int> children;
    std::string name;
    JointTransform restTransform;
    PolyMesh * mesh;
    glm::ivec3 weightIndex;
};


class SkeletalModel : public Model  {
public:
    SkeletalModel(const py::kwargs& kwargs);
    std::vector<glm::mat4> calculateCurrentPose(std::unordered_map<int, JointTransform>& pose);
    const std::vector<std::pair<int, glm::vec3>>& getOffsetPoints() const;
    int getJointId(const std::string&);
    std::shared_ptr<Renderer> getRenderer() const override;
    const std::vector<std::shared_ptr<PolyMesh>>& getModels() const;
    SkeletalAnimation* getAnimation(const std::string& id);
    /**
     *
     * Gets an array of the all important model-space transforms of all the
     * joints (with the current animation pose applied) in the entity. The
     * joints are ordered in the array based on their joint index. The position
     * of each joint's transform in the array is equal to the joint's index.
     *
     * @return The array of model-space transforms of the joints in the current
     *         animation pose.
     */
    const glm::mat4& getRestTransform(int id) const;
    glm::ivec3 getWeightIndex(int id) const;
    const JointInfo& getJointInfo(int id) const;
private:
    std::vector<JointInfo> m_jointInfos;
    std::vector<glm::mat4> m_invRestTransforms2;
    std::vector<glm::mat4> m_restTransforms2;

    std::vector<std::pair<int, glm::vec3>> m_offsetPoints;
    std::unordered_map<std::string, int> m_jointNameToId;
    std::vector<std::shared_ptr<PolyMesh>> m_models;
    std::unordered_map<std::string, std::shared_ptr<SkeletalAnimation>> m_animations;
    int m_root;
    std::string m_defaultAnimation;
};

inline const JointInfo & SkeletalModel::getJointInfo(int id) const {
    return m_jointInfos[id];
}

inline const std::vector<std::shared_ptr<PolyMesh>>& SkeletalModel::getModels() const {
    return m_models;
}

inline const std::vector<std::pair<int, glm::vec3>>& SkeletalModel::getOffsetPoints() const {
    return m_offsetPoints;
}

inline int SkeletalModel::getJointId(const std::string & id) {
    auto i = m_jointNameToId.find(id);
    if (i == m_jointNameToId.end()) {
        return -1;
    }
    return i->second;
}
