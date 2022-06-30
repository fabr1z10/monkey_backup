#pragma once
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
#include <pybind11/pybind11.h>
#include <glm/glm.hpp>
#include "room.h"
#include "shader.h"

namespace py = pybind11;


class Engine {
public:
	static Engine& instance() {
		static Engine instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	//Engine();
	void start();
	void load(pybind11::object obj);
	void shutdown();
	Shader* getShader(int);
	float getDeviceAspectRatio() const;
	glm::vec2 getDeviceSize() const;
	glm::vec4 getActualDeviceViewport() const;
	void setActualDeviceViewport(glm::vec4) ;
	static void WindowResizeCallback(GLFWwindow* win, int width, int height);
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
};

Engine& getEngine();

