#include "say.h"
#include "../node.h"
#include "../components/renderer.h"
#include "../pyhelper.h"
#include "../engine.h"
#include "../models/text.h"

Say::Say(const pybind11::kwargs& args) : NodeAction(args) {
    m_lines = args["lines"].cast<std::vector<std::string>>();
    m_direction = dictget<std::string>(args, "dir", "");
    m_parentId = args["parent"].cast<int>();
    m_pos = args["pos"].cast<glm::vec2>();
    m_fontId = args["font"].cast<std::string>();
    m_fontSize = args["size"].cast<int>();
    m_color = dictget<glm::vec4>(args, "color", glm::vec4(1.f));
    m_duration = dictget<float>(args, "duration", 0.5f);
}


void Say::start() {
	m_currentNode = nullptr;
    NodeAction::start();
    m_renderer = dynamic_cast<SpriteRenderer*>(m_node->getComponent<Renderer>());
    if (m_direction.empty()) {
        m_direction = m_renderer->getAnimation().back();
    }
    m_renderer->setAnimation("talk_" + m_direction);
    m_parent = Engine::instance().getNode(m_parentId).get();
    m_currentLine = 0;
    initLine(m_currentLine);
}

void Say::initLine(int line) {
	if (m_currentNode != nullptr) {
		m_currentNode->remove();
	}
    m_timer = 0.f;
    auto node = std::make_shared<Node>();
    node->setModel(std::make_shared<Text>(m_fontId, m_lines[line], m_fontSize));
    node->setPosition(m_pos.x, m_pos.y, 0.f);
    node->setMultColor(m_color);
    m_parent->add(node);
	m_currentNode = node.get();


}

int Say::run(double dt) {
    m_timer += static_cast<float>(dt);
    if (m_timer > m_duration) {
    	m_currentLine ++;
    	if (m_currentLine >= m_lines.size()) {
    		if (m_currentNode != nullptr) m_currentNode->remove();
			m_renderer->setAnimation("idle_" + m_direction);
    		return 0;
    	}
    	initLine(m_currentLine);
    }
    return 1;

}