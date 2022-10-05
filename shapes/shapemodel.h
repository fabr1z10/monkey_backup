#pragma once

#include "../model.h"
#include "aabb.h"
#include "../symbols.h"


std::shared_ptr<Model> AABBmodel(AABB* s, glm::vec4 color, FillType, float z=0.f);

