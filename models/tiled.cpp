#include <iostream>
#include "tiled.h"
#include "../engine.h"
#include "../asset_manager.h"
#include <stack>
#include "../pyhelper.h"

//void TiledModel::draw(Shader * s, const glm::mat4 & m) {
//    if (m_texId != GL_INVALID_VALUE) {
//        s->setInt("texture_diffuse1", 0);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, m_texId);
//    }
//    Model::draw(s, m);
//}

void TiledModel::readTiles(const std::string& inputString, std::vector<GLfloat>& vertices, std::vector<unsigned>& indices) {
    std::string u(inputString);
    std::transform(u.begin(), u.end(), u.begin(), ::toupper);
    std::vector<std::string> tokens;
    std::stringstream check1(u);
    std::string intermediate;
    // Tokenizing w.r.t. space ' '
    while(getline(check1, intermediate, ','))
    {
        tokens.push_back(intermediate);
    }
    unsigned n = 0;
    bool fliph = false;
    bool flipv = false;
    int width = -1;
    int x0 = 0;
    int y0 = 0;
    int x = x0;
    int y = y0;
    std::stack<std::pair<int, int>> loopStack;
    //unsigned i0 = vertices.size();
    bool horizontalFlip = false;
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
            x = x0;
            y += 1;
            n++;
            continue;
        }
        if (tokens[n][0] == 'H') {
            // flip horizontal next tile only
            n++;
            horizontalFlip = true;
            continue;
        }
        int tx = std::stoi(tokens[n++]);
        if (tx != -1) {
            int ty = std::stoi(tokens[n++]);
            float tx0 = tx * m_t1;
            float tx1 = (tx + 1.f) * m_t1;
            if (horizontalFlip) {
                std::swap(tx0, tx1);
                horizontalFlip = false;
            }
            // bottom left
            vertices.insert(vertices.end(),
                            {x * m_tileSize[0], y * m_tileSize[1], 0.0f, tx0, (ty + 1.f) * m_t2, 1, 1, 1, 1});
            // bottom right
            vertices.insert(vertices.end(),
                            {x * m_tileSize[0] + m_tileSize[0], y * m_tileSize[1], 0.0f, tx1, (ty + 1) * m_t2, 1, 1,
                             1, 1});
            // top right
            vertices.insert(vertices.end(),
                            {x * m_tileSize[0] + m_tileSize[0], y * m_tileSize[1] + m_tileSize[1], 0.0f, tx1,
                             ty * m_t2, 1, 1, 1, 1});
            // top left
            vertices.insert(vertices.end(),
                            {x * m_tileSize[0], y * m_tileSize[1] + m_tileSize[1], 0.0f, tx0, ty * m_t2, 1, 1, 1, 1});
            indices.insert(indices.end(), {m_i0, m_i0 + 1, m_i0 + 2, m_i0 + 3, m_i0, m_i0 + 2});
            m_i0 += 4;
        }
        x++;
        if (width != -1) {
            if (x >= width) {
                x %= width;
                y++;
            }
        }





    }
}

void TiledModel::readSheet(const pybind11::kwargs& args) {
    auto sheetId = args["sheet"].cast<int>();

    const auto& game = Engine::instance().getConfig();
    auto cane = game.attr("settings").attr("tilesets").cast<pybind11::dict>();
    auto imgName = cane[py::int_(sheetId)]["img"].cast<std::string>();
    auto ts = cane[py::int_(sheetId)]["tile_size"].cast<pybind11::tuple>();
    m_tileSize = glm::vec2(ts[0].cast<int>(), ts[1].cast<int>());
    auto& am = AssetManager::instance();
    auto tex = am.getTex(imgName);
    if (tex->hasPalette()) {
        m_shaderType = ShaderType::SHADER_TEXTURE_PALETTE;
        m_paletteId = tex->getDefaultPaletteId();
        auto palette = dictget<std::string>(args, "palette", "");
        if (!palette.empty()) {
            auto pal = am.getPalette(palette);
            m_paletteId = pal->getTexId();
        }
    }
    m_t1 = static_cast<float>(m_tileSize[0]) / tex->getWidth();
    m_t2 = static_cast<float>(m_tileSize[1]) / tex->getHeight();
    m_texId = tex->getTexId();

}

TiledModel::TiledModel() : Model(ShaderType::SHADER_TEXTURE), m_i0(0) {

}

TiledModel::TiledModel(const pybind11::kwargs& args) : Model(ShaderType::SHADER_TEXTURE), m_i0(0) {
    // Vector of string to save tokens
    readSheet(args);
    std::vector<GLfloat> vertices;
    std::vector<unsigned> indices;

    auto s = args["desc"].cast<std::string>();
    readTiles(s, vertices, indices);

    // generate buffers
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    m_size = indices.size();


}

std::shared_ptr<Renderer> AnimatedTiledModel::getRenderer() const {
    return std::make_shared<AnimatedTiledModelRenderer>();

}



AnimatedTiledModel::AnimatedTiledModel(const pybind11::kwargs& args) : TiledModel() {
    readSheet(args);
    std::vector<GLfloat> vertices;
    std::vector<unsigned> indices;
    for (const auto& frame : args["frames"]) {
        auto s = frame["desc"].cast<std::string>();
        int ticks = frame["ticks"].cast<int>();
        auto offset = indices.size();
        readTiles(s, vertices, indices);
        auto count = indices.size() - offset;
        m_frameInfo.emplace_back(offset, count, ticks);
    }
    // generate buffers
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    m_size = indices.size();
}

std::type_index AnimatedTiledModelRenderer::getType() {
    return std::type_index(typeid(Renderer));
}

void AnimatedTiledModelRenderer::start() {
    m_ticks = 0;
    m_frame = 0;
}

void AnimatedTiledModelRenderer::draw(Shader * s) {

    const auto& a = m_tiled->getFrameInfo(m_frame);
    m_model->draw(s, a.offset, a.count);
    if (m_ticks >= a.ticks) {
        // increment frame. if this animation is
        m_frame++;
        if (m_frame >= m_tiled->getFrameCount()) {
            m_frame = 0;
        }
        m_ticks = 0;
    } else {
        // if it's not time to update frame, increment current frame length
        m_ticks++;
    }

}

void AnimatedTiledModelRenderer::setModel(std::shared_ptr<Model> model) {
    Renderer::setModel(model);
    m_tiled = std::dynamic_pointer_cast<AnimatedTiledModel>(model);
}