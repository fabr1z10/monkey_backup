#pragma once


#include "keyframe.h"
#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

class SkModel;

struct SkeletalBoxInfo {
    std::string jointId;
    std::string attachPoint;
    std::string size;
};

struct AttackInfo {
    float startTime;
    float endTime;
    std::vector<SkeletalBoxInfo> boxInfos;
};

class SkeletalAnimation {
public:
    SkeletalAnimation(const YAML::Node& node);

    /**
     * @return The length of the animation in seconds.
     */
    float getLength();
    const std::vector<std::shared_ptr<KeyFrame>>& getKeyFrames();
    std::tuple<KeyFrame*, KeyFrame*, float> getPreviousAndNextKeyFrames (float t);
    const std::vector<AttackInfo>& getAttacks() const;
    std::unordered_map<std::string, glm::mat4> getAnimTransform(float, SkModel*);
    bool hasAttacks() const;
    bool loop() const;
private:
    bool m_loop;
    float m_length;
    std::vector<std::shared_ptr<KeyFrame>> m_keyFrames;
    std::vector<AttackInfo> m_attacks;
};

inline bool SkeletalAnimation::hasAttacks() const {
    return !m_attacks.empty();
}

inline bool SkeletalAnimation::loop() const {
    return m_loop;
}

inline const std::vector<std::shared_ptr<KeyFrame>>& SkeletalAnimation::getKeyFrames() {
    return m_keyFrames;
}

inline const std::vector<AttackInfo> & SkeletalAnimation::getAttacks() const {
    return m_attacks;
}