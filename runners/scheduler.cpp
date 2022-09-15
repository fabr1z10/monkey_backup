#include "scheduler.h"
#include "../pyhelper.h"

long Action::getId() const {
    return _id;
}
void Action::setId(long id) {
    _id = id;
}

//
//void Scheduler::start() {
//    for (auto& b : m_actions) {
//        b.second->init(m_node);
//    }
//}

Script::Script(const pybind11::kwargs& args) : m_done(false), _nextId(0) {
    m_scriptId = dictget<std::string>(args, "id", "");
}



long Script::add(std::shared_ptr<Action> action, const pybind11::args & args) {
    action->setId(_nextId);
    m_actions[_nextId] = action;
    m_inDegree[_nextId] = 0;
    m_edges[_nextId] = std::vector<int>();
    for (size_t i = 0 ; i< args.size(); i++) {
    	long prev =args[i].cast<long>();
    	if (prev != -1) {
			m_inDegree[_nextId]++;
			m_edges[prev].push_back(_nextId);
		}
    }

    return _nextId++;

}

void Script::kill() {
    m_done = true;
    // maybe do something more?
    for (auto& action : m_current) {
        action->end();
    }
}

void Script::update(double dt) {
    // is there anything to process? do it otherwise do nothing


    std::vector<int> complete;
    for (auto it = m_current.begin(); it != m_current.end();) {
        if ((*it)->run(dt) == 0) {
            // action is completed
            (*it)->end();
            for (const auto& edge : m_edges.at((*it)->getId())) {
                m_inDegree[edge]--;
            }
            complete.push_back((*it)->getId());
            it = m_current.erase(it);
        } else {
            it++;
        }
    }

    // check for new actions
    std::unordered_map<long, int>::iterator it = m_inDegree.begin();
    while (it != m_inDegree.end()) {
        // Check if in degree is 0
        if (it->second == 0) {
            // erase() function returns the iterator of the next
            // to last deleted element.
            auto action = m_actions.at(it->first);
            action->start();
            m_current.push_back(action);
            it = m_inDegree.erase(it);
        } else {
            it++;
        }
    }


    // cleanup
    for (auto c : complete) {
        m_inDegree.erase(c);
        m_actions.erase(c);
    }
    if (m_current.empty()) {
        m_done = true;
    }
}

void Scheduler::update(double dt) {

    // run all scripts
    for (auto it = m_scripts.begin(); it != m_scripts.end();) {
        if ((*it)->done()) {
            m_scriptMap.erase((*it)->getId());
            it = m_scripts.erase(it);
        } else {
            (*it)->update(dt);
            it++;
        }
    }


}

long Scheduler::add(std::shared_ptr<Script> s) {
    m_ids[_nextId] = s;
    m_scripts.push_back(s);
    auto sid = s->getId();
    if (!sid.empty()) {
        auto f = m_scriptMap.find(sid);
        if (f != m_scriptMap.end()) {
            f->second->get()->kill();
            m_scripts.erase(f->second);
        }
        m_scriptMap[sid] = std::prev(m_scripts.end());
    }
    return _nextId++;

}

void Scheduler::kill(long id) {
    m_ids.at(id)->kill();

}
