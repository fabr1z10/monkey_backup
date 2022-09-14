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
    void setPriority(int);
    void enter();
    void exit();
    void click(glm::vec2);
    void removeManager();


    void setShape(std::shared_ptr<Shape>);
private:
    pybind11::function m_onEnter;
    pybind11::function m_onLeave;
    pybind11::function m_onClick;

    std::shared_ptr<Shape> m_shape;
    HotSpotManager* m_manager;
    int m_priority;
};

inline int HotSpot::getPriority() const {
    return m_priority;
}

inline void HotSpot::setPriority(int priority) {
	m_priority = priority;
}