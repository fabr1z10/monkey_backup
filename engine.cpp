#include "engine.h"
#include <iostream>

#include "pyhelper.h"
#include "symbols.h"
#include "shaders/shaders.h"


GLFWwindow* window;

Engine& getEngine() {
	auto& engine = Engine::instance();
	return engine;
}

Engine::Engine() : m_room(nullptr), m_nextId(0) {

	m_shaderBuilders[0] = [&] () { add_shader<VCShader>(ShaderType::SHADER_COLOR, color_vs, color_fs); };
	m_shaderBuilders[1] = [&] () { add_shader<VTCShader>(ShaderType::SHADER_TEXTURE, tex_vs, tex_fs); };


}

void Engine::load(pybind11::object obj) {
	m_game = obj;
	m_title = m_game.attr("pippo").attr("title").cast<std::string>();
	m_windowSize = as<glm::ivec2>(m_game.attr("pippo").attr("window_size"));
	m_deviceSize = as<glm::ivec2>(m_game.attr("pippo").attr("device_size"));
	m_deviceAspectRatio = static_cast<float>(m_deviceSize[0]) / m_deviceSize[1];
	m_roomId = m_game.attr("pippo").attr("room").cast<std::string>();
	m_frameTime = 1.0 / 60.0;
	m_timeLastUpdate = 0.0;
	m_enableMouse = pyget<bool>(m_game.attr("pippo"), "enable_mouse", false);

}

void Engine::addNode(std::shared_ptr<Node> node) {
    m_allNodes[node->getId()] = node;
}

std::shared_ptr<Node> Engine::getNode(int id) {
    auto it = m_allNodes.find(id);
    if (it != m_allNodes.end()) {
        if (auto d = it->second.lock()) {
            return d;
        }
    }
    return nullptr;
}

void Engine::start() {
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		exit(1);
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( m_windowSize[0], m_windowSize[1], m_title.c_str(), NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(window);
	// note: we are setting a callback for the frame buffer resize event,
	// so the dimensions we will get will be in pixels and NOT screen coordinates!
	glfwSetFramebufferSizeCallback(window, Engine::WindowResizeCallback);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		exit(1);
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	Engine::WindowResizeCallback(window, m_windowSize[0], m_windowSize[1]);
	glfwSetKeyCallback(window, key_callback );
    if (m_enableMouse) {
        //glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, cursor_pos_callback);
        //glfwSetScrollCallback(window, scroll_callback);
        //exit(1);
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	loadShaders();
    m_shutdown = false;
    while (!m_shutdown) {
        m_roomId = m_game.attr("pippo").attr("room").cast<std::string>();
        loadRoom();
        // start up all nodes and components
        m_room->iterate_dfs([](Node *n) { n->start(); });
        m_run = true;
        m_room->start();
        do {
            double currentTime = glfwGetTime();
            /// note: if I run the update only every frame time CPU goes to 100%. If I run it on
            /// every iter, it doesn't. Tried move the glfwSwapBuffers call (and successive) out of the loop
            /// and that seems to work.
            if (currentTime - m_timeLastUpdate >= m_frameTime) {
                m_timeLastUpdate = currentTime;

                // remove all entities scheduled for removal
                if (!m_scheduledForRemoval.empty()) {
                    for (auto & g : m_scheduledForRemoval) {
                        g->getParent()->removeChild(g->getId());
                        m_allNodes.erase(g->getId());
                    }
                    m_scheduledForRemoval.clear();
                }


                // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
                //glClear(GL_COLOR_BUFFER_BIT);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // Draw nothing, see you in tutorial 2 !
                m_room->update(m_frameTime);


                for (const auto &shader : m_shaders) {
                    shader->use();
                    m_room->draw(shader.get());
                }
                //m_room->draw();
                glfwSwapBuffers(window);
                glfwPollEvents();

                // Swap buffers
            }
            m_shutdown = !(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
                         glfwWindowShouldClose(window) == 0);

        } // Check if the ESC key was pressed or the window was closed
        while (m_run && !m_shutdown);
        m_room->end();
        m_allNodes.clear();
        if (m_room) {
            m_room->cleanUp();
        }
        m_room = nullptr;
    }
	glfwTerminate();
}


void Engine::shutdown() {
	//	// Close OpenGL window and terminate GLFW
	m_shutdown = true;
	//glfwTerminate();
}

void Engine::closeRoom() {
    m_run = false;
}

void Engine::loadRoom() {
	m_title = m_game.attr("pippo").attr("title").cast<std::string>();
	//pybind11::object obj = m_game.attr("rooms").attr(m_roomId.c_str())();
	//m_room = obj.cast<std::shared_ptr<Room>>();
	m_room = m_game.attr("rooms").attr(m_roomId.c_str())().cast<std::shared_ptr<Room>>();
	//m_room = obj.cast<Room*>();
//	std::cout << room << "\n";
//	std::cout << "cane!\n";
//	std::cout << room->id() << "\n";
	//exit(1);
	std::cout << "qui\n";
}

void Engine::loadShaders() {
	auto shaders = m_game.attr("pippo").attr("shaders").cast<std::vector<int>>();
	for (auto sh : shaders) {
		m_shaderBuilders[sh]();
	}



//	exit(1);
//	auto shader = std::make_shared<VCShader>("shaders/color.vs", "shaders/color.fs");
//	m_shaders.push_back(shader);
//	m_shaders.push_back(shader);
}

// width and height will be pixels!!
void Engine::WindowResizeCallback(GLFWwindow* win, int width, int height) {
	// notify cameras
	if (height == 0) height = 1;
	std::cout << "(" << width << ", " << height << ")\n";
	float winAspectRatio = static_cast<float>(width) / height;
	auto& engine = Engine::instance();
	engine.m_windowSize = glm::ivec2(width, height);
	auto deviceSize = engine.getDeviceSize();
	auto dar = engine.getDeviceAspectRatio();
	int vx, vy, vw, vh;
	if (winAspectRatio > dar) {
		// vertical bands
		vw = (int) (height * dar);
		vh = height;
		vx = (int) ((width - vw) / 2);
		vy = 0;
	} else {
		// horizontal bands
		vw = width;
		vh = (int) (width / dar);
		vx = 0;
		vy = (int) ((height - vh) / 2);
	}
	engine.setActualDeviceViewport(glm::vec4(vx, vy, static_cast<float>(vw) / deviceSize[0], static_cast<float>(vh) / deviceSize[1]));
}

void Engine::cursor_pos_callback(GLFWwindow * win, double xpos, double ypos) {
    for (auto &listener : Engine::instance().m_mouseListeners) {
        listener->cursorPosCallback(win, xpos, ypos);
    }
}


float Engine::getDeviceAspectRatio() const {
	return m_deviceAspectRatio;
}

glm::vec2 Engine::getDeviceSize() const {
	return m_deviceSize;
}

glm::ivec2 Engine::getWindowSize() const {
    return m_windowSize;
}

glm::vec4 Engine::getActualDeviceViewport() const {
	return m_actualDeviceViewport;
}

void Engine::setActualDeviceViewport(glm::vec4 viewport) {
	m_actualDeviceViewport = viewport;
}

std::shared_ptr<Shader> Engine::getShader(ShaderType type) {
	return m_shaders[m_shaderTypeToIndex.at(type)];
}

std::shared_ptr<Room> Engine::getRoom() {
	return m_room;
}

void Engine::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// don't handle key events for disable keys until mods==16 (programmatically for demo-mode)
	//std::cout << key << ", " << scancode << ", " << action << ", " << mods << "\n";
	for (auto& s : Engine::instance().m_keyboardListeners) {
		s->keyCallback(window, key, scancode, action, mods);
	}
}

void Engine::registerToKeyboardEvent(KeyboardListener * listener) {
	m_keyboardListeners.insert(listener);
}

void Engine::registerToMouseEvent(MouseListener* listener) {
    m_mouseListeners.insert(listener);
}

void Engine::unregisterToKeyboardEvent(KeyboardListener * listener) {
	m_keyboardListeners.erase(listener);

}
void Engine::unregisterToMouseEvent(MouseListener * listener) {
    m_mouseListeners.erase(listener);

}
void Engine::scheduleForRemoval(Node * node) {
    m_scheduledForRemoval.push_back(node);
}