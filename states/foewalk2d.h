#pragma once

#include "walk2d.h"
#include "../components/controller.h"
#include "../components/dynamics.h"
#include "../components/renderer.h"

class Node;

class FoeWalk2D : public Walk2D {
public:
    FoeWalk2D(const std::string& id, const pybind11::kwargs&);
    void control() override;
private:
    bool m_flipOnEdge;
    bool m_flipHorizontally;
};


