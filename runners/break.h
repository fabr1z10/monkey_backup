#pragma once

#include "actions.h"

class Break : public NodeAction {
public:
    Break(const pybind11::kwargs&);
    void start() override;
    int run(double) override;
private:
    AnimatedRenderer* m_renderer;
    std::string m_animation;
    bool m_sync;
    int m_rows;
    int m_cols;
    std::string m_spriteContainer;
};