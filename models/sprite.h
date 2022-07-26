#pragma once

#include "../model.h"
#include "../shape.h"
#include "../hashpair.h"
#include <yaml-cpp/yaml.h>

struct AnimInfo {
	bool loop;
	int frames;
};

struct FrameInfo {
	int offset;
	int count;
	int ticks;
};

struct SpriteCollisionInfo {
    int flag;
    int mask;
    int tag;
};

class Sprite : public Model {
public:
	Sprite(const YAML::Node& node);
	void draw(Shader*, int, int) override;
	virtual std::shared_ptr<Renderer> getRenderer() const;
	const FrameInfo& getFrameInfo(const std::string& anim, int frame);
	const AnimInfo* getAnimInfo(const std::string& anim);

	std::shared_ptr<Shape> getShape (const std::string& anim, int frame) const;
    std::shared_ptr<Shape> getShapeCast (const std::string& anim, int frame) const;
    bool hasCollision(const std::string&) const;
    //const SpriteCollisionInfo& getCollisionInfo(const std::string& anim) const;
    std::shared_ptr<Model> generateDebugModel();
    Bounds getStaticBounds() const;
private:
	GLuint m_texId;
	std::string m_defaultAnimation;
	std::unordered_map<std::string, AnimInfo> m_animInfo;
	std::unordered_map<std::string, std::vector<FrameInfo>> m_frameInfo;


	// collision shapes (if any)
    std::vector<std::shared_ptr<Shape>> m_shapes;
    std::unordered_map<std::pair<std::string, int>, int> m_frameToShape;
    Bounds m_staticBounds;

};

inline Bounds Sprite::getStaticBounds() const {
    return m_staticBounds;
}

inline const AnimInfo * Sprite::getAnimInfo(const std::string &anim) {
	return &m_animInfo[anim];
}

