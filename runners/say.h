#pragma once

#include "actions.h"

class SpriteRenderer;

class Say : public NodeAction {
public:
    Say(const pybind11::kwargs&);
    int run(double) override;
    void start() override;
private:
    SpriteRenderer * m_renderer;
    std::vector<std::string> m_lines;
    std::string m_direction;
};