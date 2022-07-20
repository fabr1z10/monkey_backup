#pragma once

#include "../model.h"
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


class Sprite : public Model {
public:
	Sprite(const YAML::Node& node);
	void draw(Shader*, int, int) override;
	virtual std::shared_ptr<Renderer> getRenderer() const;
	const FrameInfo& getFrameInfo(const std::string& anim, int frame);
	const AnimInfo* getAnimInfo(const std::string& anim);
private:
	GLuint m_texId;
	std::string m_defaultAnimation;
	std::unordered_map<std::string, AnimInfo> m_animInfo;
	std::unordered_map<std::string, std::vector<FrameInfo>> m_frameInfo;
};

inline const AnimInfo * Sprite::getAnimInfo(const std::string &anim) {
	return &m_animInfo[anim];
}