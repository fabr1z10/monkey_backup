#pragma once

#include "../components/renderer.h"
#include "skeletalmodel.h"
#include "skeletalanimation.h"
#include "../jointtransform.h"
#include <vector>

struct Pose {
    std::vector<glm::mat4> bonesTransform;
    glm::mat4 offset;
};

class SkeletalRenderer : public AnimatedRenderer {
public:
    SkeletalRenderer(const std::string& defaultAnimation);
    void start() override;
    void update(double) override;
    void draw(Shader*) override;
    void setAnimation(const std::string&) override;
    float getAnimationTime() const;
    std::type_index getType() override;
    SkeletalModel* getSkeletalModel();
    void setModel(std::shared_ptr<Model>) override;
    const std::vector<glm::mat4>& getBonesTransform();
    Pose computePose(const std::string& anim, float t);
    Bounds getAttackRange() const override;

private:
    std::unordered_map<int, JointTransform> interpolatePoses(
            KeyFrame* previousFrame, KeyFrame* nextFrame, float progression);

    SkeletalModel* m_skeletalModel;
    SkeletalAnimation* m_currentAnimation;
    //std::string m_currentAnimationId;
    float m_animationTime;
    std::vector<glm::mat4> m_bones;

};

inline const std::vector<glm::mat4> & SkeletalRenderer::getBonesTransform() {
    return m_bones;
}

inline SkeletalModel * SkeletalRenderer::getSkeletalModel() {
    return m_skeletalModel;
}

inline float SkeletalRenderer::getAnimationTime() const {
    return m_animationTime;
}