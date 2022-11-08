#pragma once


#include "renderer.h"

class ClearDepthBuffer : public Renderer {
public:
    int setup(Shader*) override;
    void draw(Shader*) override;
    std::type_index getType() override;

};

inline std::type_index ClearDepthBuffer::getType() {
    return std::type_index(typeid(Renderer));
}