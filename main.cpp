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
#include <pybind11/operators.h>
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
#include "components/foewalk2d.h"
#include "components/playerwalk2d.h"
#include "shapes/aabb.h"
#include "components/smartcollider.h"
#include "components/idle.h"
#include "components/autowalk2d.h"
#include "components/selfdestroy.h"
#include "components/hotspotmanager.h"
#include "components/walkarea.h"
#include "runners/walk.h"
#include "models/image.h"
#include "runners/say.h"
#include "components/panzoom.h"
#include "components/inputtext.h"
#include "components/depth.h"
#include "skeletal/skeletalmodel.h"
#include "skeletal/skeletalcollider.h"
#include "components/attack.h"
#include "models/shapes.h"

namespace py = pybind11;

PYBIND11_MODULE(monkey, m) {
	m.doc() = "prova prova2"; // optional module docstring
	m.def("engine", &getEngine, py::return_value_policy::reference, "Gets the engine");
	m.def("get_node", &getNode);
	m.def("get_sprite", &getSprite);
    m.def("get_tiled", &getTiled);
    m.def("root",&getRoot);
    m.def("get_mesh", &getMesh);
	m.def("make_model", &makeModel);
	m.def("play", &playScript);
    m.def("close_room", &closeRoom);
	m.attr("TRIANGLES") = GL_TRIANGLES;
	m.attr("LINES") = GL_LINES;
	m.attr("SHADER_COLOR") = static_cast<int>(ShaderType::SHADER_COLOR);
	m.attr("SHADER_TEXTURE") = static_cast<int>(ShaderType::SHADER_TEXTURE);
	m.attr("FILL_OUTLINE") = static_cast<int>(FillType::OUTLINE);
	m.attr("FILL_SOLID") = static_cast<int>(FillType::SOLID);
    m.attr("HALIGN_CENTER") = static_cast<int>(HorizontalAlign::CENTER);
    m.attr("HALIGN_LEFT") = static_cast<int>(HorizontalAlign::LEFT);
    m.attr("HALIGN_RIGHT") = static_cast<int>(HorizontalAlign::RIGHT);


    py::class_<glm::vec2>(m, "vec2")
        .def_readwrite("x", &glm::vec2::x)
        .def_readwrite("y", &glm::vec2::y)
        .def(py::self + py::self)
        .def(py::init<float>())
        .def(py::init<float, float>());

	py::class_<glm::vec3>(m, "vec3")
	    .def_readwrite("x", &glm::vec3::x)
	    .def_readwrite("y", &glm::vec3::y)
	    .def_readwrite("z", &glm::vec3::z)
	    .def(py::self + py::self)
	    .def(py::init<float>())
	    .def(py::init<float, float, float>());

	py::class_<glm::vec4>(m, "vec4")
		.def_readwrite("x", &glm::vec4::x)
		.def_readwrite("y", &glm::vec4::y)
		.def_readwrite("z", &glm::vec4::z)
		.def_readwrite("w", &glm::vec4::w)
		.def(py::self + py::self)
		.def(py::init<float>())
		.def(py::init<float, float, float, float>());

	py::class_<Engine>(m, "Engine")
		//.def(py::init<>())
		.def("start", &Engine::start)
		.def("shutdown", &Engine::shutdown)
		.def("load",&Engine::load)
        .def("get_node", &Engine::getNode, py::return_value_policy::reference);
		//.def("instance", &Engine::instance, py::return_value_policy::reference);

	py::class_<Node, std::shared_ptr<Node>>(m, "Node")
		.def(py::init<>())
		.def("children_count", &Node::getChildrenCount)
		.def("set_model", &Node::setModel)
		.def("set_state", &Node::setState)
		.def_property_readonly("state", &Node::getState)
		.def("get_camera", &Node::getCamera)
		.def("set_camera", &Node::setCamera)
		.def("set_position", &Node::setPosition)
		.def("get_animation", &Node::getAnimation)
		.def("set_animation", &Node::setAnimation)
		.def("set_text", &Node::setText)
		.def("add_component", &Node::addComponent)
		.def("set_mult_color", &Node::setMultColor)
		.def("set_add_color", &Node::setAddColor)
		.def_property("user_data", &Node::getUserData, &Node::setUserData)
		.def_property("active", &Node::active, &Node::setActive)
		.def("get_parent",&Node::getParent, py::return_value_policy::reference)
        .def("get_dynamics", &Node::getComponent<Dynamics>, py::return_value_policy::reference)
    	.def("get_hotspot", &Node::getComponent<HotSpot>, py::return_value_policy::reference)
        .def("get_move_dynamics", &Node::getComponent<MoveDynamics>, py::return_value_policy::reference)
        .def("get_controller", &Node::getComponent<Controller>, py::return_value_policy::reference)
        .def("get_collider", &Node::getComponent<Collider>, py::return_value_policy::reference)
		.def("get_renderer", &Node::getComponent<Renderer>, py::return_value_policy::reference)
		.def("get_keyboard", &Node::getComponent<Keyboard>, py::return_value_policy::reference)
		.def("clear_children", &Node::clearChildren)
		.def("get_children", &Node::getChildren)
		.def("set_scale", &Node::setScale)
        .def_property_readonly("flip_x",&Node::getFlipX)
		.def_property_readonly("position", &Node::getPos)
		.def_property_readonly("id", &Node::getId)
		.def_property_readonly("x", &Node::getX)
		.def_property_readonly("y", &Node::getY)
		.def("add", &Node::add)
	    .def("remove", &Node::remove);

	/* models */
    py::module_ mm = m.def_submodule("models");
	py::class_<Model, std::shared_ptr<Model>>(mm, "Model")
		.def(py::init<int>());
	py::class_<RawModel, Model, std::shared_ptr<RawModel>>(mm, "RawModel")
		.def(py::init<int, const py::array_t<float>&, const py::array_t<unsigned>&, const py::kwargs&>());
	py::class_<Sprite, Model, std::shared_ptr<Sprite>>(mm, "sprite");
    py::class_<SkeletalModel, Model, std::shared_ptr<SkeletalModel>>(mm, "skeletal_model")
        .def(py::init<const pybind11::kwargs&>());
    py::class_<PolyMesh, Model, std::shared_ptr<PolyMesh>>(mm, "polymesh");
    py::class_<TiledModel, Model, std::shared_ptr<TiledModel>>(mm, "tiled")
        .def(py::init<const std::string&>());
    py::class_<Image, Model, std::shared_ptr<Image>>(mm, "image")
        .def(py::init<const std::string&, const pybind11::kwargs&>());
	py::class_<Text, Model, std::shared_ptr<Text>>(mm, "text")
	    .def_property_readonly("size", &Text::getSize)
		.def(py::init<const py::kwargs&>());
    py::class_<LineModel, Model, std::shared_ptr<LineModel>>(mm, "line")
        .def(py::init<const pybind11::kwargs&>());
    py::class_<RectModel, Model, std::shared_ptr<RectModel>>(mm, "rect")
        .def(py::init<const pybind11::kwargs&>());
    py::class_<Pixels, Model, std::shared_ptr<Pixels>>(mm, "pixels")
        .def(py::init<const pybind11::kwargs&>());


	py::class_<Camera, std::shared_ptr<Camera>>(m, "camera")
	    .def("set_bounds", &Camera::setBounds)
	    .def("set_position", &Camera::setPosition)
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

    py::class_<AABB, Shape, std::shared_ptr<AABB>>(m, "aabb")
        .def(py::init<float, float, float, float>());

    py::class_<CompoundShape, Shape, std::shared_ptr<CompoundShape>>(m, "compound_shape")
        .def("add_shape", &CompoundShape::addShape)
        .def(py::init<>());

	/// --- components ---
	py::class_<Component, std::shared_ptr<Component>>(m, "component");

    py::class_<KeyboardListener, std::shared_ptr<KeyboardListener>>(m, "keyboard_listener");

	py::class_<Collider, Component, std::shared_ptr<Collider>>(m, "icollider")
	    .def("set_collision_flag", &Collider::setCollisionFlag);

	py::class_<SimpleCollider, Collider, std::shared_ptr<SimpleCollider>>(m, "collider")
		.def(py::init<std::shared_ptr<Shape>, int, int, int>());

    py::class_<SpriteCollider, Collider, std::shared_ptr<SpriteCollider>>(m, "sprite_collider")
        .def(py::init<int, int, int, const pybind11::kwargs&>());

    py::class_<SkeletalCollider, Collider, std::shared_ptr<SkeletalCollider>>(m, "skeletal_collider")
        .def(py::init<int, int, int>());


    py::class_<HotSpotManager, Component, std::shared_ptr<HotSpotManager>>(m, "hot_spot_manager")
        .def(py::init<>());

	py::class_<PanZoom, Component, std::shared_ptr<PanZoom>>(m, "panzoom")
		.def(py::init<>());

	py::class_<Depth, Component, std::shared_ptr<Depth>>(m, "depth")
		.def(py::init<const pybind11::kwargs&>());


	py::class_<HotSpot, Component, std::shared_ptr<HotSpot>>(m, "hotspot")
    	.def("set_shape", &HotSpot::setShape)
    	.def_property("priority", &HotSpot::getPriority, &HotSpot::setPriority)
        .def(py::init<std::shared_ptr<Shape>, const pybind11::kwargs&>());

	py::class_<Move, Component, std::shared_ptr<Move>>(m, "move")
		.def(py::init<py::function>());

    py::class_<MoveQuat, Component, std::shared_ptr<MoveQuat>>(m, "movequat")
        .def(py::init<const pybind11::kwargs&>());

	py::class_<Renderer, Component, std::shared_ptr<Renderer>>(m, "renderer")
		.def("flip", &Renderer::flipHorizontal)
		.def(py::init<>());


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
        .def_readwrite("velocity", &Dynamics::m_velocity)
		.def(py::init<const pybind11::kwargs&>());

	py::class_<Follow, Component, std::shared_ptr<Follow>>(m, "follow")
		.def(py::init<std::shared_ptr<Camera>, pybind11::tuple&, pybind11::tuple&>());


	py::class_<Controller, Component, std::shared_ptr<Controller>>(m, "controller")
        .def("set_size", &Controller::setSize)
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

	py::class_<InputText, Component, KeyboardListener, std::shared_ptr<InputText>>(m, "input_text")
		.def(py::init<const std::string&, float, int, const py::kwargs&>());

    py::class_<Platform, Component, std::shared_ptr<Platform>>(m, "platform")
        .def(py::init<>());

    py::class_<SelfDestroy, Component, std::shared_ptr<SelfDestroy>>(m, "self_destroy")
        .def(py::init<py::kwargs&>());

    py::class_<WalkArea, Component, std::shared_ptr<WalkArea>>(m, "walkarea")
        .def(py::init<const py::kwargs&>());


    /// --- actions ---
    py::module_ ma = m.def_submodule("actions");
    py::class_<Action, std::shared_ptr<Action>>(ma, "action");
    py::class_<NodeAction, Action, std::shared_ptr<NodeAction>>(ma, "node_action");
	py::class_<Animate, NodeAction, std::shared_ptr<Animate>>(ma, "animate")
		.def(py::init<const pybind11::kwargs&>());
    py::class_<MoveBy, NodeAction, std::shared_ptr<MoveBy>>(ma, "move_by")
        .def(py::init<const pybind11::kwargs&>());
    py::class_<MoveAccelerated, NodeAction, std::shared_ptr<MoveAccelerated>>(ma, "move_accelerated")
        .def(py::init<const pybind11::kwargs&>());
    py::class_<SetState, NodeAction, std::shared_ptr<SetState>>(ma, "set_state")
        .def(py::init<const pybind11::kwargs&>());
    py::class_<Delay, Action, std::shared_ptr<Delay>>(ma, "delay")
        .def(py::init<float>());
    py::class_<WaitForKey, Action, std::shared_ptr<WaitForKey>>(ma, "wait_for_key")
        .def(py::init<int>());
    py::class_<Blink, NodeAction, std::shared_ptr<Blink>>(ma, "blink")
        .def(py::init<const pybind11::kwargs&>());
    py::class_<Walk, NodeAction, std::shared_ptr<Walk>>(ma, "walk")
        .def(py::init<const pybind11::kwargs&>());
    py::class_<Say, NodeAction, std::shared_ptr<Say>>(ma, "say")
        .def(py::init<const pybind11::kwargs&>());
    py::class_<RemoveNode, NodeAction, std::shared_ptr<RemoveNode>>(ma, "remove")
        .def(py::init<const pybind11::kwargs&>());
    py::class_<AddNode, NodeAction, std::shared_ptr<AddNode>>(ma, "add")
        .def(py::init<const pybind11::kwargs&>());
    py::class_<CallFunc, Action, std::shared_ptr<CallFunc>>(ma, "callfunc")
        .def(py::init<pybind11::function>());
	py::class_<Repeat, Action, std::shared_ptr<Repeat>>(ma, "repeat")
		.def(py::init<pybind11::function, float>());
    py::class_<RevealText, NodeAction, std::shared_ptr<RevealText>>(ma, "reveal_text")
        .def(py::init<const pybind11::kwargs&>());
    py::class_<Script, std::shared_ptr<Script>>(m, "script")
        .def("add", &Script::add)
        .def(py::init<const pybind11::kwargs&>());

	/// --- states ---
	py::class_<State, std::shared_ptr<State>>(m, "state");

    py::class_<Walk2D, State, std::shared_ptr<Walk2D>>(m, "walk_2d");

	py::class_<PlayerWalk2D, State, std::shared_ptr<PlayerWalk2D>>(m, "walk_2d_player")
		.def(py::init<const std::string&, py::kwargs&>());

    py::class_<FoeWalk2D, State, std::shared_ptr<FoeWalk2D>>(m, "walk_2d_foe")
        .def(py::init<const std::string&, py::kwargs&>());

    py::class_<AutoWalk2D, State, std::shared_ptr<AutoWalk2D>>(m, "walk_2d_auto")
        .def(py::init<const std::string&, py::kwargs&>());

    py::class_<Idle, State, std::shared_ptr<Idle>>(m, "idle")
        .def(py::init<const std::string&, const std::string&, py::kwargs&>());

    py::class_<Attack, State, std::shared_ptr<Attack>>(m, "attack")
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
		.def("set_on_start", &Room::setOnStart)
		.def("set_on_end", &Room::setOnEnd)
	    .def("root", &Room::getRoot, py::return_value_policy::reference);

//	py::module_ submodule = m.def_submodule("my_submodule");
//    submodule.def("hello_submodule", []()
//    { std::cout << "Hello from submodule" << std::endl; });
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

