#include "skeletalrenderer.h"
#include <glm/gtx/transform.hpp>
#include "../node.h"

SkeletalRenderer::SkeletalRenderer(const std::string& animId) : AnimatedRenderer(animId), m_skeletalModel(nullptr), m_currentAnimation(nullptr) {}

std::type_index SkeletalRenderer::getType() {
    return std::type_index(typeid(Renderer));
}


void SkeletalRenderer::start() {
    if (!m_animation.empty()) {
        m_currentAnimation = m_skeletalModel->getAnimation(m_animation);
    }
    m_animationTime = 0.f;
}


void SkeletalRenderer::update(double dt) {
    std::unordered_map<int, JointTransform> pose;
    if (m_currentAnimation != nullptr) {
        m_animationTime += static_cast<float>(dt);
        if (m_animationTime >= m_currentAnimation->getLength()) {
            m_complete = true;
            if (m_currentAnimation->loop()) {
                m_animationTime = fmod(m_animationTime, m_currentAnimation->getLength());
            }  else {
                m_animationTime = m_currentAnimation->getLength() - 0.001f;
            }
        }
        // compute current pose
        auto frames = m_currentAnimation->getPreviousAndNextKeyFrames(m_animationTime);
        pose = interpolatePoses(std::get<0>(frames), std::get<1>(frames), std::get<2>(frames));
    }

    m_bones = m_skeletalModel->calculateCurrentPose(pose);

    // apply offset
    const auto& offsetPoints = m_skeletalModel->getOffsetPoints();
    if (!offsetPoints.empty()) {
        glm::vec3 offset(0.0f);
        //std::cout << "no of offset points: " << offsetPoints.size() << "\n";
        for (const auto &a : offsetPoints) {
            // find coordinates of offset pointg
            glm::vec4 p = m_bones[a.first] * glm::vec4(a.second, 1.0f);
            offset.y = std::max(-p.y, offset.y);
        }
        //std::cerr << offset.y << "\n";
        setTransform(glm::translate(offset));
    }

}


void SkeletalRenderer::draw(Shader * s) {
    const auto& m = m_node->getWorldMatrix() * m_rendererTransform;
    s->setVec4("mult_color", m_multColor);
    s->setVec4("add_color", m_addColor);
    s->setMat4("model", m);
    if (s->getShaderType() == ShaderType::SHADER_SKELETAL) {
        auto boneLoc = glGetUniformLocation(s->getProgId(), "Bone");
        auto l2m = glGetUniformLocation(s->getProgId(), "local_to_model");
        auto weightIndex = glGetUniformLocation(s->getProgId(), "weightIndex");
        auto pz = glGetUniformLocation(s->getProgId(), "z");
        glUniformMatrix4fv(boneLoc, m_bones.size(), false, &m_bones[0][0][0]);
        int n{0};
        for (const auto& model : m_skeletalModel->getModels()) {
            auto restTransform = m_skeletalModel->getRestTransform(n);
            const auto& jinfo = m_skeletalModel->getJointInfo(n++);
            auto weightIndices = jinfo.weightIndex;// m_skeletalModel->getWeightIndex(n++);
            glUniformMatrix4fv(l2m, 1, false, &restTransform[0][0]);
            glUniform3iv(weightIndex, 1, &weightIndices[0]);
            glUniform1f(pz, jinfo.z);
            model->draw(s, 0, 0);

        }
    }
//            auto boneLoc = glGetUniformLocation(s->(), "Bone");
//            auto l2m = glGetUniformLocation(s->getProgId(), "local_to_model");
//            auto weightIndex = glGetUniformLocation(s->getProgId(), "weightIndex");
//            glUniformMatrix4fv(boneLoc, m_bones.size(), false, &m_bones[0][0][0]);
//            int n = 0;
//            for (const auto& mesh : *m_model) {
//                // set local to model
//                auto restTransform = m_spriteModel->getRestTransform(n);
//                auto weightIndices = m_spriteModel->getWeightIndex(n++);
//                glUniformMatrix4fv(l2m, 1, false, &restTransform[0][0]);
//                glUniform3iv(weightIndex, 1, &weightIndices[0]);
//                mesh->draw(s, 0, 0);
//            }
//        }
//
//    }
}

void SkeletalRenderer::setModel(std::shared_ptr<Model> model) {
    Renderer::setModel(model);
    m_skeletalModel = dynamic_cast<SkeletalModel*>(model.get());
    assert(m_skeletalModel != nullptr);
}

std::unordered_map<int, JointTransform> SkeletalRenderer::interpolatePoses(
        KeyFrame* previousFrame, KeyFrame* nextFrame, float progression) {
    std::unordered_map<int, JointTransform> currentPose;
    const auto& nf = nextFrame->getJointKeyFrames();
    for (const auto& p : previousFrame->getJointKeyFrames()) {
        // the model might not have this joint ... in this case nothing to do
        int jointId = m_skeletalModel->getJointId(p.first);
        if (jointId == -1) {
            continue;
        }

        // previousTransform is p.second
        JointTransform nextTransform = nf.at(p.first);
        //JointTransform currentTransform = m_model->getRestTransform(p.first);
        auto localTransform = JointTransform::interpolate(p.second, nextTransform, progression);
        //currentTransform += JointTransform::interpolate(p.second, nextTransform, progression);
        //currentTransform.z = 0;
        //std::cout << m_animationTime << " . " << currentTransform.alpha << "\n";
        currentPose.insert(std::make_pair(jointId, localTransform));
    }
    return currentPose;
}


void SkeletalRenderer::setAnimation(const std::string & id) {
    if (id == m_animation) {
        return;
    }

    m_complete = false;
    m_currentAnimation = m_skeletalModel->getAnimation(id);
    m_animation = id;
    m_animationTime = 0.0f;
}
