#pragma once

#include <unordered_map>
#include "models/sprite.h"
#include "font.h"
#include "tex.h"
#include "models/tiled.h"
#include "models/spritesheet.h"
#include "models/polymesh.h"
#include "skeletal/skeletalanimation.h"
#include "palette.h"


class AssetManager {
private:
public:
	static AssetManager& instance() {
		static AssetManager instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	std::shared_ptr<Sprite> getSprite(const std::string&);
    std::shared_ptr<TiledModel> getTiled(const std::string&);
	std::shared_ptr<Tex> getTex(const std::string&);
    std::shared_ptr<Palette> getPalette(const std::string&);
	std::shared_ptr<Font> getFont(const std::string&);
    std::shared_ptr<PolyMesh> getPolymesh(const std::string&);
    std::shared_ptr<SkeletalAnimation> getSkeletalAnimation(const std::string&);

private:

    std::pair<std::string, std::string> splitFileAsset(const std::string&);
    std::unordered_map<std::string, std::shared_ptr<TiledModel>> m_tiled;
	std::unordered_map<std::string, std::shared_ptr<Sprite>> m_sprites;
    std::unordered_map<std::string, std::shared_ptr<PolyMesh>> m_polymesh;
    std::unordered_map<std::string, std::shared_ptr<SkeletalAnimation>> m_skeletalAnimations;
	std::unordered_map<std::string, std::shared_ptr<SpriteSheet>> m_spriteSheets;
	std::unordered_map<std::string, std::shared_ptr<Tex>> m_tex;
    std::unordered_map<std::string, std::shared_ptr<Palette>> m_palettes;
	std::unordered_map<std::string, std::shared_ptr<Font>> m_fonts;

};