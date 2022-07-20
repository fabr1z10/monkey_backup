#include <iostream>
#include "tiled.h"
#include "../engine.h"
#include "../asset_manager.h"
#include <stack>

TiledModel::TiledModel(int tilesetId, const pybind11::list& l) : Model(ShaderType::SHADER_TEXTURE) {

    const auto& game = Engine::instance().getConfig();
    auto cane = game.attr("pippo").attr("tilesets").cast<pybind11::dict>();
    auto imgName = cane[py::int_(0)]["img"].cast<std::string>();
    auto ts = cane[py::int_(0)]["tile_size"].cast<pybind11::tuple>();
    glm::vec2 tileSize(ts[0].cast<int>(), ts[1].cast<int>());

    auto& am = AssetManager::instance();
    auto tex = am.getTex(imgName);
    float t1 = static_cast<float>(tileSize[0]) / tex->getWidth();
    float t2 = static_cast<float>(tileSize[1]) / tex->getHeight();
    m_texId = tex->getTexId();

    std::vector<GLfloat> vertices;
    std::vector<unsigned> indices;

    bool fliph = false;
    bool flipv = false;

    std::stack<std::pair<int, int>> loopStack;

    int x = 0;
    int y = 0;
    bool end = false;
    int n = 0;
    while (n < l.size()) {
        try {
            auto a = l[n].cast<std::string>();
            // start loop
            if (a == "R") {
                loopStack.push(std::make_pair(n+2, l[n+1].cast<int>()-1));
                n += 2;
            }
            // end loop
            if (a == "E") {
                auto& t = loopStack.top();
                if (t.second > 0) {
                    t.second -= 1;
                    n = t.first;
                } else {
                    loopStack.pop();
                    n++;
                }
            }
            if (a == "FH[") {
                fliph = true; n++;
            }
            if (a == "FH]") {
                fliph = false;n++;
            }
            if (a == "FV[") {
                flipv = true;n++;
            }
            if (a == "FV]") {
                flipv = false;n++;
            }

            // command
        } catch (...) {
            int tx = l[n++].cast<int>();
            int ty = l[n++].cast<int>();
            std::cout << "(" << tx << ", " << ty << ")\n";
            vertices.insert(vertices.end(), {x * tileSize[0], y * tileSize[1], 0.0f, tx, ty + th, 1, 1, 1, 1});

        }
    }

    std::cout << imgName << " " << m_texId <<" ok \n";
    exit(1);


}
