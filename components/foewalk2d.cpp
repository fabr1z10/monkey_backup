#include "foewalk2d.h"
#include "../pyhelper.h"

void FoeWalk2D::control() {

    if ( (m_left && m_controller->left()) || (m_right && m_controller->right())) {
        m_left = !m_left;
        m_right = !m_left;
    }
    //if (m_flipIfPlatformEnds) {
    // check if I reached the end of the platform
    if (m_controller->grounded() && m_controller->isFalling(m_left ? -1.f : 1.f)) {
        m_left = !m_left;
        m_right = !m_left;

    }
}

FoeWalk2D::FoeWalk2D(const std::string& id, const pybind11::kwargs& kwargs) : Walk2D(id, kwargs) {
    m_left = dictget<bool>(kwargs, "left", true);
    m_right = !m_left;
    m_up = false;
}


