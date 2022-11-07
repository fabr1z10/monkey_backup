#pragma once

#include "renderer.h"
#include "controller.h"

class Shadow : public Renderer {
public:
    Shadow(const pybind11::kwargs&);
    int setup(Shader*) override;

    void draw(Shader*) override;
    void start() override;
    void update(double) override;


    std::type_index getType() override;
private:
    float m_ground;
    float m_shear; // shadow elongation along x axis
    float m_length;
    Renderer* m_parentRenderer;
    Node* m_parent;
    Controller2D* m_controller;
};