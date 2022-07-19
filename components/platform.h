#pragma once

#include "../component.h"
#include <unordered_set>
#include "glm/glm.hpp"
#include "../runners/collisionengine.h"

class Controller2D;

class Platform : public Component {
public:
    Platform() {}
    void start() override;
    void registerComponent(Controller2D*);
    void unregisterComponent(Controller2D*);
    void unregisterAll();
    void move(Node*);
private:
    std::unordered_set<Controller2D*> m_characters;
    glm::vec3 m_lastPosition;
    CollisionEngine* m_collisionEngine;
    float m_platformWidth;

};