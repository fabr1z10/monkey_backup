#pragma once

#include "walk2d.h"
#include "controller.h"
#include "dynamics.h"
#include "renderer.h"

class Node;

class FoeWalk2D : public Walk2D {
public:
    FoeWalk2D(const std::string& id, const pybind11::kwargs&);
    void control() override;
private:
    bool m_flipOnEdge;
};


