#pragma once

#include "../model.h"
#include "../components/renderer.h"

struct TiledFrameInfo {
    TiledFrameInfo(int a, int b, int c) : offset(a), count(b), ticks(c) {}
    unsigned offset;
    unsigned count;
    int ticks;
};

class TiledModel : public Model {
public:
    TiledModel();
    TiledModel(const pybind11::kwargs&);
    //void draw(Shader*, const glm::mat4&) override;
protected:
    void readSheet(const pybind11::kwargs&);
    void readTiles(const std::string& inputString, std::vector<GLfloat>& vertices, std::vector<unsigned>& indices);
    glm::vec2 m_tileSize;
    float m_t1, m_t2;
    unsigned m_i0;
};


class AnimatedTiledModel : public TiledModel {
public:
    AnimatedTiledModel(const pybind11::kwargs&);
    const TiledFrameInfo& getFrameInfo(int) const;
    size_t getFrameCount() const;
    std::shared_ptr<Renderer> getRenderer() const override;

private:
    std::vector<TiledFrameInfo> m_frameInfo;

};

inline const TiledFrameInfo & AnimatedTiledModel::getFrameInfo(int i) const {
    return m_frameInfo[i];
}

inline size_t AnimatedTiledModel::getFrameCount() const {
    return m_frameInfo.size();
}

class AnimatedTiledModelRenderer : public Renderer {
public:
    AnimatedTiledModelRenderer() = default;
    void setModel(std::shared_ptr<Model>) override;
    std::type_index getType() override;
    void start() override;
    //virtual int getFrame() const;
    void draw(Shader *) override;
private:
    std::shared_ptr<AnimatedTiledModel> m_tiled;
    int m_frame;
    std::shared_ptr<Sprite> m_sprite;
    int m_ticks;
};
