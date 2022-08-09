#pragma once

#include "../component.h"
#include "../shape.h"
#include <pybind11/pybind11.h>


class HotSpotManager;

class HotSpot : public Component {
public:
    HotSpot (std::shared_ptr<Shape>, const pybind11::kwargs&);
    virtual ~HotSpot();
    void start() override;
    bool isMouseInside(glm::vec2);
    int getPriority() const;
    void enter();
    void exit();
private:
    pybind11::function m_onEnter;
    pybind11::function m_onLeave;
    std::shared_ptr<Shape> m_shape;
    HotSpotManager* m_manager;
    int m_priority;
};

inline int HotSpot::getPriority() const {
    return m_priority;
}