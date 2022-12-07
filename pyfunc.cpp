#include "pyfunc.h"
#include "asset_manager.h"
#include <iostream>
#include "modelmake.h"
#include "engine.h"

std::shared_ptr<Sprite> getSprite(const std::string& id) {
	auto& am = AssetManager::instance();
	return am.getSprite(id);
}

std::shared_ptr<Node> getNode(int id) {
	return Engine::instance().getNode(id);
}

std::shared_ptr<TiledModel> getTiled(const std::string& id) {
    auto& am = AssetManager::instance();
    return am.getTiled(id);
}

std::shared_ptr<PolyMesh> getMesh(const std::string& id) {
    auto& am = AssetManager::instance();
    return am.getPolymesh(id);
}

std::shared_ptr<Model> makeModel(std::shared_ptr<Shape> shape, const pybind11::kwargs& args) {
	auto& m = ModelMaker::instance();
	return m.get(shape, args);
}

int playScript(const std::shared_ptr<Script> s) {
    return Engine::instance().getRoom()->getRunner<Scheduler>()->add(s);
}

void closeRoom() {
    Engine::instance().closeRoom();
}

std::shared_ptr<Node> getRoot() {
    return Engine::instance().getRoom()->getRoot();
}