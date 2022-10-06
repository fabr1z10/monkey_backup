#include "skeletalanimation.h"


SkeletalAnimation::SkeletalAnimation(const YAML::Node& node) {
    m_loop = node["loop"].as<bool>(true);
    m_length = node["length"].as<float>(0.f);
    auto joints = node["joints"].as<std::vector<std::string>>();
    //auto speedUp = 1.0f / t.get<float>("speed_up", 1.0f);
    int index = 0;
    for (const auto& keyFrame : node["key_frames"]) {
        auto t = keyFrame["t"].as<float>();
        auto data = keyFrame["data"].as<std::vector<float>>();
        std::unordered_map<std::string, JointTransform> pose;
        for (size_t i = 0, j = 0 ; i < data.size(); i+=3, ++j) {
            JointTransform tr;
            tr.translation = glm::vec3(data[i], data[i+1], 0.f);
            tr.rotation = glm::angleAxis(glm::radians(data[i+2]), glm::vec3(0.f, 0.f, 1.f));
            tr.scale = glm::vec3(1.f);
            pose[joints[j]] = tr;
        }
        m_keyFrames.push_back(std::make_shared<KeyFrame>(index, t, pose));
        index++;
    }
    if (!m_loop) {
        m_length = m_keyFrames.back()->getTimeStamp();
    }

    // attack boxes
    if (node["attacks"]) {
        for (const auto& attack : node["attacks"]) {
            auto startTime = attack[0].as<float>();
            auto endTime = attack[1].as<float>();
            m_attacks.emplace_back(startTime, endTime);
        }
    }
}



float SkeletalAnimation::getLength() {
    return m_length;
}

std::tuple<KeyFrame*, KeyFrame*, float> SkeletalAnimation::getPreviousAndNextKeyFrames(float t) {
    if (t < 0 || t > m_length) {
        return std::make_tuple(nullptr, nullptr, 0.0f);
    }
    for (int i = 1; i < m_keyFrames.size(); ++i) {
        if (m_keyFrames[i]->getTimeStamp() > t) {
            float progression = (t - m_keyFrames[i-1]->getTimeStamp()) / (m_keyFrames[i]->getTimeStamp() -m_keyFrames[i-1]->getTimeStamp());
            return std::make_tuple(m_keyFrames[i-1].get(), m_keyFrames[i].get(), progression);
        }
    }
    // if we get here, we are in a loop
    float progression = (t - m_keyFrames.back()->getTimeStamp()) / (m_length - m_keyFrames.back()->getTimeStamp());
    return std::make_tuple(m_keyFrames.back().get(), m_keyFrames.front().get(), progression);

}

int SkeletalAnimation::getAttack(float u) const {
    int id = 0;
    for (const auto& t : m_attacks) {
        if (u >= t.first && u <= t.second) {
            return id;
        }
        if (u > t.second) {
            return -1;
        }
        id++;
    }
    return -1;
}