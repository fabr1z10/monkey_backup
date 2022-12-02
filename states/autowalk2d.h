#include "walk2d.h"
#include <list>


struct AutoInputEvent {
    float t;
    bool left;
    bool right;
    bool up;
    pybind11::function callback;
};


class AutoWalk2D : public Walk2D {
public:
    AutoWalk2D(const std::string& id, const pybind11::kwargs&);
    void init(const pybind11::kwargs& args) override;
    void control() override;
    void run(double) override;
private:
    std::list<AutoInputEvent> m_inputEvents;
    float m_timer;
};


