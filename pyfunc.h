#pragma once

#include "models/sprite.h"
#include "shape.h"
#include "models/tiled.h"
#include "runners/scheduler.h"
#include "models/polymesh.h"

std::shared_ptr<Sprite> getSprite(const std::string&);
std::shared_ptr<Node> getNode(int);
std::shared_ptr<TiledModel> getTiled(const std::string&);
std::shared_ptr<PolyMesh> getMesh(const std::string&);
int playScript(std::shared_ptr<Script>);
std::shared_ptr<Model> makeModel(std::shared_ptr<Shape>, const pybind11::kwargs&);
