#include <iostream>
#include "tiled.h"
#include "../engine.h"
#include "../asset_manager.h"
#include <stack>

TiledModel::TiledModel(const std::string& s) : Model(ShaderType::SHADER_TEXTURE) {
    // Vector of string to save tokens
    std::string u(s);
    std::transform(u.begin(), u.end(), u.begin(), ::toupper);
    std::vector <std::string> tokens;
    std::cout << "called TILED!\n";
    // stringstream class check1
    std::stringstream check1(u);

    std::string intermediate;

    // Tokenizing w.r.t. space ' '
    while(getline(check1, intermediate, ','))
    {
        tokens.push_back(intermediate);
    }
    int sheetId = std::stoi(tokens[0]);
    // first token is the sheet id
    const auto& game = Engine::instance().getConfig();
    auto cane = game.attr("pippo").attr("tilesets").cast<pybind11::dict>();
    auto imgName = cane[py::int_(sheetId)]["img"].cast<std::string>();
    auto ts = cane[py::int_(0)]["tile_size"].cast<pybind11::tuple>();
    glm::vec2 tileSize(ts[0].cast<int>(), ts[1].cast<int>());
    auto& am = AssetManager::instance();
    auto tex = am.getTex(imgName);
    float t1 = static_cast<float>(tileSize[0]) / tex->getWidth();
    float t2 = static_cast<float>(tileSize[1]) / tex->getHeight();
    m_texId = tex->getTexId();

    unsigned n = 1;

    std::vector<GLfloat> vertices;
    std::vector<unsigned> indices;
    bool fliph = false;
    bool flipv = false;
    int width = -1;
    int x = 0;
    int y = 0;
    std::stack<std::pair<int, int>> loopStack;
    unsigned i0 = 0;
    while (n < tokens.size()) {
        if (tokens[n][0] == 'W') {
            width = std::stoi(tokens[n+1]);
            n += 2;
            continue;
        }

        if (tokens[n][0] == 'R') {
            loopStack.push(std::make_pair(n+2, std::stoi(tokens[n+1])-1));
            n += 2;
            continue;
        }
        if (tokens[n][0] == 'E') {
            auto& t = loopStack.top();
            if (t.second > 0) {
                t.second -= 1;
                n = t.first;
            } else {
                loopStack.pop();
                n++;
            }
            continue;
        }
        if (tokens[n][0] == 'U') {
            x = 0;
            y += 1;
            n++;
            continue;
        }
        int tx = std::stoi(tokens[n++]);
        int ty = std::stoi(tokens[n++]);

        // bottom left
        vertices.insert(vertices.end(), {x * tileSize[0], y * tileSize[1], 0.0f, tx * t1, (ty + 1.f) * t2, 1, 1, 1, 1});
        // bottom right
        vertices.insert(vertices.end(), {x * tileSize[0] + tileSize[0], y * tileSize[1], 0.0f, (tx + 1.f) * t1, (ty + 1) * t2, 1, 1, 1, 1});
        // top right
        vertices.insert(vertices.end(), {x * tileSize[0] + tileSize[0], y * tileSize[1] + tileSize[1], 0.0f, (tx + 1.f) * t1, ty * t2, 1, 1, 1, 1});
        // top left
        vertices.insert(vertices.end(), {x * tileSize[0], y * tileSize[1] + tileSize[1], 0.0f, tx * t1, ty * t2, 1, 1, 1, 1});
        indices.insert(indices.end(), {i0, i0 + 1, i0 + 2, i0 + 3, i0, i0 + 2});
        i0 += 4;
        x++;
        if (width != -1) {
            if (x >= width) {
                x %= width;
                y++;
            }
        }





    }

    // generate buffers
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    m_size = indices.size();




    //bool end = false;
    //int n = 0;

//    while (n < l.size()) {
//        try {
//            auto a = l[n].cast<std::string>();
//            // start loop
//            if (a == "R") {
//                loopStack.push(std::make_pair(n+2, l[n+1].cast<int>()-1));
//                n += 2;
//            }
//            // end loop
//            if (a == "E") {
//                auto& t = loopStack.top();
//                if (t.second > 0) {
//                    t.second -= 1;
//                    n = t.first;
//                } else {
//                    loopStack.pop();
//                    n++;
//                }
//            }
//            if (a == "FH[") {
//                fliph = true; n++;
//            }
//            if (a == "FH]") {
//                fliph = false;n++;
//            }
//            if (a == "FV[") {
//                flipv = true;n++;
//            }
//            if (a == "FV]") {
//                flipv = false;n++;
//            }
//
//            // command
//        } catch (...) {
//            int tx = l[n++].cast<int>();
//            int ty = l[n++].cast<int>();
//            std::cout << "(" << tx << ", " << ty << ")\n";
//            // bottom left
//
//            vertices.insert(vertices.end(), {x * tileSize[0], y * tileSize[1], 0.0f, tx * t1, (ty + 1.f) * t2, 1, 1, 1, 1});
//            // bottom right
//            vertices.insert(vertices.end(), {x * tileSize[0] + tileSize[0], y * tileSize[1], 0.0f, (tx + 1.f) * t1, (ty + 1) * t2, 1, 1, 1, 1});
//            // top right
//            vertices.insert(vertices.end(), {x * tileSize[0] + tileSize[0], y * tileSize[1] + tileSize[1], 0.0f, (tx + 1.f) * t1, ty * t2, 1, 1, 1, 1});
//            // top left
//            vertices.insert(vertices.end(), {x * tileSize[0], y * tileSize[1] + tileSize[1], 0.0f, tx * t1, ty * t2, 1, 1, 1, 1});
//            indices.insert(indices.end(), {i0, i0 + 1, i0 + 2, i0 + 3, i0, i0 + 2});
//            i0 += 4;
//            x++;
//        }
//    }
//
//    //std::cout << imgName << " " << m_texId <<" ok \n";
//    //exit(1);
//    // generate buffers
//    glGenBuffers(1, &m_vbo);
//    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
//
//    glGenBuffers(1, &m_ebo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
//
//    m_size = indices.size();

}
