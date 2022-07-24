#include "foewalk2d.h"
#include "../pyhelper.h"

void FoeWalk2D::control() {

}

FoeWalk2D::FoeWalk2D(const std::string& id, const pybind11::kwargs& kwargs) : Walk2D(id, kwargs) {
    m_left = dictget<bool>(kwargs, "left", true);
    m_right = !m_left;
    m_up = false;
}


