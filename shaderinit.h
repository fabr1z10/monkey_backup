#pragma once

#include "shader.h"

class LightInitializer : public ShaderInitializer {
public:
    void init(Shader*) override;
};

class LightPreDraw : public ShaderPreDraw {
public:
    void init(Shader*, Node*) override;
};

