#pragma once

#include "../model.h"
#include <yaml-cpp/yaml.h>

class PolyMesh : public Model {
public:
    PolyMesh(const YAML::Node& node);
    glm::vec2 getKeyPoint(const std::string&);
    glm::vec2 getDimension(const std::string&);
private:
    //GLuint m_texId;
    glm::vec2 m_origin;
    std::unordered_map<std::string, glm::vec2> m_keyPoints;
    std::unordered_map<std::string, glm::vec2> m_dimensions;
    void cioa(const std::string& id, std::unordered_map<std::string, glm::vec2>& m, const YAML::Node& node);
};

