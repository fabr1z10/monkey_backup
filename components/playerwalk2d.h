#include "walk2d.h"


class PlayerWalk2D : public Walk2D {
public:
    PlayerWalk2D(const std::string& id, const pybind11::kwargs&);
    void control() override;

    void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;

};


