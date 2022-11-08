#include "break.h"
#include "../node.h"
#include "../components/collider.h"
#include "../models/image.h"
#include <filesystem>

Break::Break(const pybind11::kwargs& args) : NodeAction(args) {
    auto size = args["size"].cast<glm::ivec2>();
    m_rows = args["rows"].cast<int>();
    m_cols = args["cols"].cast<int>();
    m_spriteContainer = args["sprite"].cast<std::string>();
}

void Break::start() {
    NodeAction::start();
    auto bounds = m_node->getComponent<Collider>()->getStaticBounds();
    std::cout << bounds.min.x << ", " << bounds.max.x << ", " <<bounds.min.y << ", " << bounds.max.y << "\n";
    float cellWidth = (bounds.max.x - bounds.min.x) / m_cols;
    float cellHeight = (bounds.max.y - bounds.min.y) / m_rows;
    float x0 = bounds.min.x + 0.5f * cellWidth;
    float y0 = bounds.min.y + 0.5f * cellHeight;
    for (size_t i = 0; i < m_rows; ++i) {
        for (size_t j = 0; j <m_cols; ++j) {
            glm::vec2( x0 + j * cellWidth, y0+ i * cellHeight);
            auto node = std::make_shared<Node>();
            //node->setModel(std::make_shared<Image>()

        }
    }
}

int Break::run(double) {
    return 0;
}
