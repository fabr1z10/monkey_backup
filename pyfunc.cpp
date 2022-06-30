#include "pyfunc.h"
#include "asset_manager.h"
#include <iostream>

std::shared_ptr<Sprite> getSprite(const std::string& id) {
	auto& am = AssetManager::instance();
	return am.getSprite(id);
}