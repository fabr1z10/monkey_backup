#pragma once
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
#include <pybind11/pybind11.h>
#include <glm/glm.hpp>
#include "room.h"
#include "shader.h"
#include "keylistener.h"
#include "mouselistener.h"

namespace py = pybind11;


class Engine {
public:
	static Engine& instance() {
		static Engine instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	//Engine();
	long getNextId();
	void start();
	void closeRoom();
	void load(pybind11::object obj);
	void shutdown();
	std::shared_ptr<Node> getNode(int);
	void addNode(std::shared_ptr<Node>);
	//Shader* getShader(int);
	float getDeviceAspectRatio() const;
	glm::vec2 getDeviceSize() const;
	glm::ivec2 getWindowSize() const;
	glm::vec4 getActualDeviceViewport() const;
	void setActualDeviceViewport(glm::vec4) ;
	static void WindowResizeCallback(GLFWwindow* win, int width, int height);
    static void cursor_pos_callback(GLFWwindow*, double xpos, double ypos);

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void registerToKeyboardEvent(KeyboardListener*);
	void unregisterToKeyboardEvent(KeyboardListener*);
    void registerToMouseEvent(MouseListener*);
    void unregisterToMouseEvent(MouseListener*);
	std::shared_ptr<Shader> getShader(ShaderType type);
	std::shared_ptr<Room> getRoom();
	pybind11::handle getConfig();
	bool isRunning() const;
	void scheduleForRemoval(Node*);
private:
	Engine();
	void loadRoom();
	void loadShaders();
	pybind11::handle m_game;
	//pybind11::object m_game;
	std::string m_title;
	std::string m_roomId;
	glm::ivec2 m_windowSize;
	glm::ivec2 m_deviceSize;
	glm::vec4 m_actualDeviceViewport;
	float m_deviceAspectRatio;
	// the current room
	std::shared_ptr<Room> m_room;
	std::vector<std::shared_ptr<Shader>> m_shaders;

	template<typename T>
	void add_shader(ShaderType type, const std::string& vertex, const std::string& fragment) {
		auto shader = std::make_shared<T>(type, vertex, fragment);
		m_shaderTypeToIndex[type] = m_shaders.size();
		m_shaders.push_back(shader);


	}
	std::unordered_map<ShaderType, int> m_shaderTypeToIndex;
	std::unordered_map<int, std::function<void()>> m_shaderBuilders;
	double m_frameTime;
	double m_timeLastUpdate;
	std::unordered_set<KeyboardListener*> m_keyboardListeners;
	std::unordered_set<MouseListener*> m_mouseListeners;
    bool m_run;
    bool m_shutdown;
    std::vector<Node*> m_scheduledForRemoval;
    long m_nextId;
    std::unordered_map<int, std::weak_ptr<Node>> m_allNodes;
    bool m_enableMouse;
};

Engine& getEngine();

inline bool Engine::isRunning() const {
    return m_run;
}

inline pybind11::handle Engine::getConfig() {
	return m_game;
}

inline long Engine::getNextId() {
    return m_nextId++;
}

