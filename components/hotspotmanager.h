#pragma once


#include "../component.h"
#include "../mouselistener.h"
#include "hotspot.h"

class OrthoCamera;

class HotSpotManager : public Component, public MouseListener {
public:
    HotSpotManager() = default;
    void cursorPosCallback(GLFWwindow*, double, double) override;
    void start() override;
    void add(HotSpot*);
    void remove(HotSpot*);
private:
    OrthoCamera* m_camera;
    std::unordered_set<HotSpot*> m_hotspot;
    HotSpot* m_previous;
};
