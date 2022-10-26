#include "actions.h"
#include "../node.h"
#include "../engine.h"
#include <glm/gtx/transform.hpp>
#include <utility>
#include "../pyhelper.h"
#include "../components/statemachine.h"
#include "../components/renderer.h"

NodeAction::NodeAction(const pybind11::kwargs& args) {
    m_node = nullptr;
    if (args.contains("id")) {
        m_nodeId = args["id"].cast<int>();
    } else {
        m_node = args["node"].cast<std::shared_ptr<Node>>().get();
    }
}

void NodeAction::start() {
    Action::start();
    if (m_node == nullptr) {
        m_node = Engine::instance().getNode(m_nodeId).get();
    }
}

Animate::Animate(const pybind11::kwargs& args) : NodeAction(args) {
	m_animation = args["anim"].cast<std::string>();
	m_sync = dictget(args, "sync", false);

}

void Animate::start() {
	NodeAction::start();
	m_renderer = static_cast<AnimatedRenderer*>(m_node->getComponent<Renderer>());
	m_renderer->setAnimation(m_animation);

}

int Animate::run(double) {
    if (!sync) return 0;
    return (m_renderer->isComplete() ? 0 : 1);
}

MoveBy::MoveBy(const pybind11::kwargs& args) : NodeAction(args) {
    auto dx = dictget<float>(args, "x", 0.f);
    auto dy = dictget<float>(args, "y", 0.f);
    auto dz = dictget<float>(args, "z", 0.f);
    auto t = dictget<float>(args, "t", -1.f);
    m_distance = sqrt(dx * dx + dy * dy);
    m_delta = glm::vec3(dx, dy, 0.f);
    m_unitVec = glm::normalize(m_delta);
    if (t < 0) {
        m_speed = args["speed"].cast<float>();
    } else {
        m_speed = m_distance / t;
    }

}

MoveAccelerated::MoveAccelerated(const pybind11::kwargs& args) : NodeAction(args) {
    m_initialVelocity = args["velocity"].cast<glm::vec3>();
    m_velocity = m_initialVelocity;
    m_acceleration = args["acceleration"].cast<glm::vec3>();
    m_timeOut = dictget<float>(args, "timeout", std::numeric_limits<float>::infinity());
    m_time = 0.f;
}

void MoveBy::start() {
    NodeAction::start();

    m_endPoint = m_node->getWorldPosition() + m_delta;
    m_distanceTraveled = 0.f;
}


int MoveBy::run(double dt) {
    auto dtf = static_cast<float>(dt);
    auto dist = m_speed * dtf;
    m_distanceTraveled += dist;
    auto delta = m_unitVec * dist;
    if (m_distanceTraveled >= m_distance) {
        m_node->setPosition(m_endPoint.x, m_endPoint.y, m_endPoint.z);
        return 0;
    }
    m_node->move(glm::translate(delta));

    return 1;
}

void MoveAccelerated::start() {
    NodeAction::start();

}

int MoveAccelerated::run(double dt) {
    auto dtf = static_cast<float>(dt);
    if (m_time > m_timeOut)
        return 0;
    glm::vec2 delta = m_velocity * dtf;
    m_velocity += m_acceleration * dtf;
    m_node->move(glm::translate(glm::vec3(delta, 0.f)));
    m_time += dtf;
    return 1.f;
}

SetState::SetState(const pybind11::kwargs& args) : NodeAction(args), m_args(args) {
    m_state = args["state"].cast<std::string>();


}

int SetState::run(double) {
    m_node->getComponent<StateMachine>()->setState(m_state, m_args);
    return 0;
}

Delay::Delay(float t) : Action(), m_time(t) {}

void Delay::start() {
    m_timer = 0.f;
}

int Delay::run(double dt) {
    m_timer += static_cast<float>(dt);
    if (m_timer > m_time) {
        return 0;
    }
    return 1;
}

Blink::Blink(const pybind11::kwargs & args) : NodeAction(args) {
    m_duration = args["duration"].cast<float>();
    m_period = args["period"].cast<float>();
}

void Blink::start() {
    NodeAction::start();
    m_renderer = m_node->getComponent<Renderer>();
    m_time = 0.f;
}

void Blink::end() {
    m_renderer->setMultColor(glm::vec4(1.f));

}

int Blink::run(double dt) {
    m_time += static_cast<float>(dt);
    if (m_time > m_duration) {
        return 0.f;
    }
    int a = static_cast<int>(m_time / m_period) % 2;
    m_renderer->setMultColor(a == 0 ? glm::vec4(1.f) : glm::vec4(0.f));

    return 1.f;
}

int RemoveNode::run(double) {
    m_node->remove();
    return 0;
}

AddNode::AddNode(const pybind11::kwargs& args) : NodeAction(args) {
    m_nodeToAdd = args["node"].cast<std::shared_ptr<Node>>();
}

int AddNode::run(double) {
    m_node->add(m_nodeToAdd);
    return 0;
}


CallFunc::CallFunc(pybind11::function f) {
    m_func = std::move(f);
}

Repeat::Repeat(pybind11::function f, float period) {
	m_func = std::move(f);
	m_period = period;
	m_timer = 0.f;
}



int CallFunc::run(double) {
    m_func();
    return 0;
}

int Repeat::run(double dt) {
	m_timer += static_cast<float>(dt);
	if (m_timer >= m_period) {
		m_timer = 0.f;
		m_func();
	}
	return 1;
}

RevealText::RevealText(const pybind11::kwargs& args) : NodeAction(args) {
    m_time = args["interval"].cast<float>();
}

int RevealText::run(double dt) {
    auto dtf = static_cast<float>(dt);
    m_timer += dtf;
    int retval = 1;
    if (m_timer > m_time) {
        m_timer = 0.f;
        m_currentCount += 6;
        if (m_currentCount >= m_elSize) {
            m_currentCount = m_elSize;
            retval = 0;
        }
    }
    m_renderer->setCount(m_currentCount);
    return retval;
}

void RevealText::start() {
    NodeAction::start();
    m_renderer = m_node->getComponent<Renderer>();
    m_elSize = m_renderer->getModel()->getElSize();
    m_currentCount = 6;
    m_timer = 0.f;
}


WaitForKey::WaitForKey(int key) : Action(), m_key(key), m_done(false) {}

int WaitForKey::run(double) {
    return (m_done ? 0 : 1);
}

void WaitForKey::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
    if (_status == 1 && key == m_key && action == GLFW_PRESS) m_done = true;
}