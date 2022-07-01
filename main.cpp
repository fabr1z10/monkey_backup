// Include standard headers
#include <stdio.h>
#include <stdlib.h>


#include "engine.h"
#include "room.h"
#include "camera.h"
#include "sprite.h"
#include "symbols.h"

//GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "pyfunc.h"

namespace py = pybind11;

PYBIND11_MODULE(monkey, m) {
	m.doc() = "prova prova2"; // optional module docstring
	m.def("engine", &getEngine, py::return_value_policy::reference, "Gets the engine");
	m.def("get_sprite", &getSprite);
	m.attr("TRIANGLES") = GL_TRIANGLES;
	m.attr("LINES") = GL_LINES;
	m.attr("SHADER_COLOR") = static_cast<int>(ShaderType::SHADER_COLOR);
	m.attr("SHADER_TEXTURE") = static_cast<int>(ShaderType::SHADER_TEXTURE);

	py::class_<Engine>(m, "Engine")
		//.def(py::init<>())
		.def("start", &Engine::start)
		.def("shutdown", &Engine::shutdown)
		.def("load",&Engine::load);
		//.def("instance", &Engine::instance, py::return_value_policy::reference);

	py::class_<Node, std::shared_ptr<Node>>(m, "Node")
		.def(py::init<>())
		.def("children_count", &Node::getChildrenCount)
		.def("set_model", &Node::setModel)
		.def("set_camera", &Node::setCamera)
		.def("set_position", &Node::setPosition)
		.def("add", &Node::add);

	py::class_<Model, std::shared_ptr<Model>>(m, "Model")
		.def(py::init<int>());

	py::class_<RawModel, Model, std::shared_ptr<RawModel>>(m, "RawModel")
		.def(py::init<int, py::array_t<float>, py::array_t<unsigned>, const py::kwargs&>());

	py::class_<Sprite, Model, std::shared_ptr<Sprite>>(m, "sprite")
		.def(py::init<const std::string&>());


	py::class_<Camera, std::shared_ptr<Camera>>(m, "camera")
		.def(py::init<const py::kwargs&>());

	py::class_<OrthoCamera, Camera, std::shared_ptr<OrthoCamera>>(m, "camera_ortho")
		.def(py::init<float, float, const py::kwargs&>());



	py::class_<Room, std::shared_ptr<Room>>(m, "Room")
	    .def(py::init<const std::string&>())
	    .def("root", &Room::getRoot, py::return_value_policy::reference);

}
//int main( void )
//{
//	// Initialise GLFW
//	if( !glfwInit() )
//	{
//		fprintf( stderr, "Failed to initialize GLFW\n" );
//		getchar();
//		return -1;
//	}
//
//	glfwWindowHint(GLFW_SAMPLES, 4);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//	// Open a window and create its OpenGL context
//	window = glfwCreateWindow( 1024, 768, "Tutorial 01", NULL, NULL);
//	if( window == NULL ){
//		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
//		getchar();
//		glfwTerminate();
//		return -1;
//	}
//	glfwMakeContextCurrent(window);
//
//	// Initialize GLEW
//	if (glewInit() != GLEW_OK) {
//		fprintf(stderr, "Failed to initialize GLEW\n");
//		getchar();
//		glfwTerminate();
//		return -1;
//	}
//
//	// Ensure we can capture the escape key being pressed below
//	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
//
//	// Dark blue background
//	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
//
//	do{
//		// Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
//		glClear( GL_COLOR_BUFFER_BIT );
//
//		// Draw nothing, see you in tutorial 2 !
//
//
//		// Swap buffers
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//
//	} // Check if the ESC key was pressed or the window was closed
//	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
//		   glfwWindowShouldClose(window) == 0 );
//
//	// Close OpenGL window and terminate GLFW
//	glfwTerminate();
//
//	return 0;
//}

