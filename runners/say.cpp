#include "say.h"
#include "../node.h"
#include "../components/renderer.h"
#include "../pyhelper.h"

Say::Say(const pybind11::kwargs& args) : NodeAction(args) {

    m_lines = args["lines"].cast<std::vector<std::string>>();
    m_direction = dictget<std::string>(args, "dir", "");
}


void Say::start() {
    NodeAction::start();
    m_renderer = dynamic_cast<SpriteRenderer*>(m_node->getComponent<Renderer>());
    if (m_direction.empty()) {
        m_direction = m_renderer->getAnimation().back();
    }
    m_renderer->setAnimation("talk_" + m_direction);
}


int Say::run(double) {

}