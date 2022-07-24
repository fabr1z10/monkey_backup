#include "pyfunc.h"
#include "asset_manager.h"
#include <iostream>
#include "modelmake.h"
#include "engine.h"

std::shared_ptr<Sprite> getSprite(const std::string& id) {
	auto& am = AssetManager::instance();
	return am.getSprite(id);
}

std::shared_ptr<TiledModel> getTiled(const std::string& id) {
    auto& am = AssetManager::instance();
    return am.getTiled(id);
}

std::shared_ptr<Model> makeModel(std::shared_ptr<Shape> shape, const pybind11::kwargs& args) {
	ModelMaker m;
	return m.get(shape, args);
}

void playScript(const std::shared_ptr<Script> s) {
    Engine::instance().getRoom()->getRunner<Scheduler>()->add(s);
}