#pragma once

#include "../model.h"
#include "../shape.h"
#include "../hashpair.h"
#include "spritesheet.h"
#include <yaml-cpp/yaml.h>


struct FrameInfo {
	int offset;
	int count;
	int ticks;
	int box;
	int attackBox;
};

struct AnimInfo {
    bool loop;
    std::vector<FrameInfo> frameInfo;
    int frameCount;
};


struct SpriteCollisionInfo {
    int flag;
    int mask;
    int tag;
};


//class ItemizedModel : public Model {
//public:
//	ItemizedModel() : Model() {}
//	//void draw(Shader*, const glm::mat4&) override;
//	virtual void innerDraw(Shader*, const glm::mat4& m, const std::string& key) = 0;
//	void addStateItem(const std::string&, int);
//protected:
//	GLuint m_texId;
//	std::vector<std::string> m_stateKeys;
//	// associate a key (fro
//	std::unordered_map<std::string, std::vector<int>> m_stateToItems;
//	std::unordered_map<std::pair<std::string, int>, glm::mat4> m_transforms;
//};
//
//
//class AnimatedModel : public ItemizedModel {
//public:
//	AnimatedModel(ShaderType shaderType, GLenum prim) : ItemizedModel() {
//		m_shaderType = shaderType;
//		m_primitive = prim;
//	}
//	void addItem(int, int);
//	virtual std::shared_ptr<Renderer> getRenderer() const;
//	void innerDraw(Shader*, const glm::mat4& m, const std::string& key) override ;
//
//private:
//	std::vector<std::pair<int, int>> m_itemInfo;
//
//};



class Sprite : public Model {
public:
    Sprite(ShaderType type, GLenum primitive);
	Sprite(const YAML::Node& node, const std::string& sheetFile);
	void init(Node*) override;
	//void draw(Shader*, Node*) override;
	std::shared_ptr<Renderer> getRenderer() const override;
	const FrameInfo& getFrameInfo(const std::string& anim, int frame);
	const AnimInfo* getAnimInfo(const std::string& anim);

	std::shared_ptr<Shape> getShape (const std::string& anim, int frame) const;
    std::shared_ptr<Shape> getShapeCast (const std::string& anim, int frame) const;
    bool hasCollision(const std::string&) const;
    //const SpriteCollisionInfo& getCollisionInfo(const std::string& anim) const;
    std::shared_ptr<Model> generateDebugModel();
    Bounds getStaticBounds() const;
//	void innerDraw(Shader*, const glm::mat4&, const std::string& key) override;
    std::string getDefaultAnimation() const;
    std::pair<int, int> getDebugShape(const std::string& anim, int frame);
    std::pair<int, int> getDebugAttackShape(const std::string& anim, int frame);

private:

	SpriteSheet* m_sheet;
	std::string m_defaultAnimation;
	std::unordered_map<std::string, AnimInfo> m_animInfo;
	//std::unordered_map<std::string, std::vector<FrameInfo>> m_frameInfo;


	// collision shapes (if any)
    std::vector<std::shared_ptr<Shape>> m_shapes;
    std::unordered_map<std::pair<std::string, int>, int> m_frameToShape;
    Bounds m_staticBounds;
    std::unordered_map<int, std::pair<int, int>> m_shapeInfo;
};

inline std::string Sprite::getDefaultAnimation() const {
    return m_defaultAnimation;
}

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

