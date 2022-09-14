#pragma once

#include "../model.h"
#include "../shape.h"
#include "../hashpair.h"
#include "spritesheet.h"
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


class ItemizedModel : public Model {
public:
	ItemizedModel() : Model() {}
	//void draw(Shader*, const glm::mat4&) override;
	virtual void innerDraw(Shader*, const glm::mat4& m, const std::string& key) = 0;
	void addStateItem(const std::string&, int);
protected:
	GLuint m_texId;
	std::vector<std::string> m_stateKeys;
	// associate a key (fro
	std::unordered_map<std::string, std::vector<int>> m_stateToItems;
	std::unordered_map<std::pair<std::string, int>, glm::mat4> m_transforms;
};


class AnimatedModel : public ItemizedModel {
public:
	AnimatedModel(ShaderType shaderType, GLenum prim) : ItemizedModel() {
		m_shaderType = shaderType;
		m_primitive = prim;
	}
	void addItem(int, int);
	virtual std::shared_ptr<Renderer> getRenderer() const;
	void innerDraw(Shader*, const glm::mat4& m, const std::string& key) override ;

private:
	std::vector<std::pair<int, int>> m_itemInfo;

};



class Sprite : public ItemizedModel {
public:
	Sprite(const YAML::Node& node, SpriteSheet*);
	void init(Node*) override;
	//void draw(Shader*, Node*) override;
	virtual std::shared_ptr<Renderer> getRenderer() const;
	const FrameInfo& getFrameInfo(const std::string& anim, int frame);
	const AnimInfo* getAnimInfo(const std::string& anim);

	std::shared_ptr<Shape> getShape (const std::string& anim, int frame) const;
    std::shared_ptr<Shape> getShapeCast (const std::string& anim, int frame) const;
    bool hasCollision(const std::string&) const;
    //const SpriteCollisionInfo& getCollisionInfo(const std::string& anim) const;
    std::shared_ptr<Model> generateDebugModel();
    Bounds getStaticBounds() const;
	void innerDraw(Shader*, const glm::mat4&, const std::string& key) override;

private:

	SpriteSheet* m_sheet;
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
    auto it = m_animInfo.find(anim);
    if (it == m_animInfo.end())
        return nullptr;

    return &it->second;
	//return &m_animInfo[anim];
}

