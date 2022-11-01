#pragma once

#include "../model.h"


class Plane : public Model {
public:
    Plane(const pybind11::kwargs& args);

};
