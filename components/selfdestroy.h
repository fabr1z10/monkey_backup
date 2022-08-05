#pragma once

#include "../component.h"
#include <pybind11/pybind11.h>

class SelfDestroy : public Component {
public:
    SelfDestroy(const pybind11::kwargs&);
    void update(double) override;
    void start() override;
private:
    float m_timer;
    float m_timeout;
    float m_yMin;
};