#pragma once

#include "actions.h"

class SpriteRenderer;

class Say : public NodeAction {
public:
    Say(const pybind11::kwargs&);
    int run(double) override;
    void start() override;
private:
    void initLine(int);
    SpriteRenderer * m_renderer;
    std::vector<std::string> m_lines;
    std::string m_direction;
    int m_parentId;
    Node* m_parent;
    Node *m_currentNode;
    float m_duration;
    float m_timer;
    int m_currentLine;
    std::string m_fontId;
    int m_fontSize;
    glm::vec2 m_pos;
    glm::vec4 m_color;
};