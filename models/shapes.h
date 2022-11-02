#pragma once

#include "../model.h"
#include "../components/renderer.h"

class Pixels : public Model {
public:
    explicit Pixels(const pybind11::kwargs& args);
    std::shared_ptr<Renderer> getRenderer() const override;

    //virtual void draw(Shader* shader, int offset, int count);
    class PixelRenderer : public Renderer {
    public:
        std::type_index getType() override {
            return std::type_index(typeid(Renderer));
        }
        void draw(Shader *) override;
    };
};

class LineModel : public Model {
public:
    LineModel(const pybind11::kwargs& args);

};

class RectModel : public Model {
public:
    RectModel(const pybind11::kwargs& args);
};

class PolygonModel : public Model {
public:
    PolygonModel(const std::vector<glm::vec2>& data);
};