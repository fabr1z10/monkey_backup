// Include standard headers
#include <stdio.h>
#include <stdlib.h>


#include "engine.h"
#include "room.h"
#include "camera.h"
#include "models/sprite.h"
#include "models/rawmodel.h"
#include "models/text.h"
#include "symbols.h"
#include "shapes/convexpoly.h"
#include "components/collider.h"
#include "runners/collisionengine.h"

//GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "pyfunc.h"
#include "components/move.h"
#include "components/controller.h"
#include "components/statemachine.h"
#include "components/walk2d.h"
#include "components/car2d.h"
#include "components/follow.h"
#include "shapes/circle.h"
#include "shapes/compound.h"
#include "components/keyboard.h"
#include "components/platform.h"
#include "models/tiled.h"
#include "runners/scheduler.h"
#include "runners/actions.h"

namespace py = pybind11;

PYBIND11_MODULE(monkey, m) {
	m.doc() = "prova prova2"; // optional module docstring
	m.def("engine", &getEngine, py::return_value_policy::reference, "Gets the engine");
	m.def("get_sprite", &getSprite);
    m.def("get_tiled", &getTiled);
	m.def("make_model", &makeModel);
	m.def("play", &playScript);
	m.attr("TRIANGLES") = GL_TRIANGLES;
	m.attr("LINES") = GL_LINES;
	m.attr("SHADER_COLOR") = static_cast<int>(ShaderType::SHADER_COLOR);
	m.attr("SHADER_TEXTURE") = static_cast<int>(ShaderType::SHADER_TEXTURE);

	py::class_<Engine>(m, "Engine")
		//.def(py::init<>())
		.def("start", &Engine::start)
		.def("shutdown", &Engine::shutdown)
		.def("close_room", &Engine::closeRoom)
		.def("load",&Engine::load);
		//.def("instance", &Engine::instance, py::return_value_policy::reference);

	py::class_<Node, std::shared_ptr<Node>>(m, "Node")
		.def(py::init<>())
		.def("children_count", &Node::getChildrenCount)
		.def("set_model", &Node::setModel)
		.def("set_camera", &Node::setCamera)
		.def("set_position", &Node::setPosition)
		.def("set_animation", &Node::setAnimation)
		.def("add_component", &Node::addComponent)
		.def("set_mult_color", &Node::setMultColor)
		.def_property("user_data", &Node::getUserData, &Node::setUserData)
		.def("get_parent",&Node::getParent, py::return_value_policy::reference)
        .def("get_move_dynamics", &Node::getComponent<MoveDynamics>, py::return_value_policy::reference)
		.def_property_readonly("position", &Node::getPos)
		.def_property_readonly("id", &Node::getId)
		.def("add", &Node::add)
	    .def("remove", &Node::remove);

	py::class_<Model, std::shared_ptr<Model>>(m, "Model")
		.def(py::init<int>());

	py::class_<RawModel, Model, std::shared_ptr<RawModel>>(m, "RawModel")
		.def(py::init<int, const py::array_t<float>&, const py::array_t<unsigned>&, const py::kwargs&>());

	py::class_<Sprite, Model, std::shared_ptr<Sprite>>(m, "sprite");

    py::class_<TiledModel, Model, std::shared_ptr<TiledModel>>(m, "tiled")
        .def(py::init<const std::string&>());

	py::class_<Text, Model, std::shared_ptr<Text>>(m, "text")
		.def(py::init<const py::kwargs&>());

	py::class_<Camera, std::shared_ptr<Camera>>(m, "camera")
		.def(py::init<const py::kwargs&>());

	py::class_<OrthoCamera, Camera, std::shared_ptr<OrthoCamera>>(m, "camera_ortho")
		.def(py::init<float, float, const py::kwargs&>());

	py::class_<PerspectiveCamera, Camera, std::shared_ptr<PerspectiveCamera>>(m, "camera_perspective")
		.def(py::init<const py::kwargs&>());

	py::class_<Shape, std::shared_ptr<Shape>>(m, "shape")
		.def(py::init<>());

	py::class_<ConvexPoly, Shape, std::shared_ptr<ConvexPoly>>(m, "convex_poly")
		.def(py::init<const py::array_t<float>&>());

	py::class_<Rect, ConvexPoly, std::shared_ptr<Rect>>(m, "rect")
		.def(py::init<float, float, const py::kwargs&>());

    py::class_<Circle, Shape, std::shared_ptr<Circle>>(m, "circle")
        .def(py::init<float, const py::kwargs&>());

    py::class_<Segment, Shape, std::shared_ptr<Segment>>(m, "segment")
        .def(py::init<float, float, float, float>());


    py::class_<CompoundShape, Shape, std::shared_ptr<CompoundShape>>(m, "compound_shape")
        .def("add_shape", &CompoundShape::addShape)
        .def(py::init<>());

	/// --- components ---
	py::class_<Component, std::shared_ptr<Component>>(m, "component");

    py::class_<KeyboardListener, std::shared_ptr<KeyboardListener>>(m, "keyboard_listener");

	py::class_<Collider, Component, std::shared_ptr<Collider>>(m, "icollider");

	py::class_<SimpleCollider, Collider, std::shared_ptr<SimpleCollider>>(m, "collider")
		.def(py::init<std::shared_ptr<Shape>, int, int, int>());

	py::class_<Move, Component, std::shared_ptr<Move>>(m, "move")
		.def(py::init<py::function>());

    py::class_<MoveDynamics, Component, std::shared_ptr<MoveDynamics>>(m, "move_dynamics")
        .def("set_velocity", &MoveDynamics::setVelocity)
        .def("set_callback", &MoveDynamics::setCallback)
        .def("set_min_y", &MoveDynamics::setMinY)
        .def("set_max_y", &MoveDynamics::setMaxY)
        .def("add_elastic_force", &MoveDynamics::addElasticForce)
        .def("set_constant_force", &MoveDynamics::setConstantForce)
        .def("add_elastic_force", &MoveDynamics::addElasticForce)
        .def(py::init<float>());


	py::class_<Dynamics, Component, std::shared_ptr<Dynamics>>(m, "dynamics")
		.def(py::init<>());

	py::class_<Follow, Component, std::shared_ptr<Follow>>(m, "follow")
		.def(py::init<std::shared_ptr<Camera>, pybind11::tuple&, pybind11::tuple&>());


	py::class_<Controller, Component, std::shared_ptr<Controller>>(m, "controller")
	    .def(py::init<const py::kwargs&>());

	py::class_<Controller2D, Controller, std::shared_ptr<Controller2D>>(m, "controller_2d")
		.def(py::init<py::kwargs&>());

	py::class_<StateMachine, Component, std::shared_ptr<StateMachine>>(m, "state_machine")
		.def("add", &StateMachine::addState)
		.def("set_initial_state", &StateMachine::setInitialState)
		.def(py::init<>());

    py::class_<Keyboard, Component, KeyboardListener, std::shared_ptr<Keyboard>>(m, "keyboard")
        .def("add", &Keyboard::addFunction)
        .def(py::init<>());

    py::class_<Platform, Component, std::shared_ptr<Platform>>(m, "platform")
        .def(py::init<>());


    /// --- actions ---
    py::class_<Action, std::shared_ptr<Action>>(m, "action");
    py::class_<MoveBy, Action, std::shared_ptr<MoveBy>>(m, "move_by")
        .def(py::init<const pybind11::kwargs&>());

    py::class_<Script, std::shared_ptr<Script>>(m, "script")
        .def("add", &Script::add)
        .def(py::init<const pybind11::args&>());

	/// --- states ---
	py::class_<State, std::shared_ptr<State>>(m, "state");

	py::class_<Walk2D, State, std::shared_ptr<Walk2D>>(m, "walk_2d")
		.def(py::init<const std::string&, py::kwargs&>());

	py::class_<Car2D, State, std::shared_ptr<Car2D>>(m, "car_2d")
		.def(py::init<const std::string&, py::kwargs&>());

	/// --- runners ---
	py::class_<Runner, std::shared_ptr<Runner>>(m, "runner");

	py::class_<CollisionEngine, Runner, std::shared_ptr<CollisionEngine>>(m, "collision_engine")
		.def("add_response", &CollisionEngine::addResponse)
		.def(py::init<float, float>());

    py::class_<Scheduler, Runner, std::shared_ptr<Scheduler>>(m, "scheduler")
        .def("add", &Scheduler::add)
        .def(py::init<>());


	py::class_<Room, std::shared_ptr<Room>>(m, "Room")
	    .def(py::init<const std::string&>())
		.def("add_runner", &Room::addRunner)
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

