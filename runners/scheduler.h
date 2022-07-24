#pragma once

#include "../runner.h"
#include <list>
#include <memory>
#include <unordered_map>
#include <pybind11/pybind11.h>

class Node;

class Action {
public:
    // return 0 when completed
    virtual void start() = 0;
    virtual int run(double) = 0;
    long getId() const;
    void setId(long);
    void init(Node*);

protected:
    long _id;
    Node* m_node;
};

class Script {
public:
    Script(const pybind11::args&);
    long add(std::shared_ptr<Action> action, const pybind11::args&);
    void update(double);
    bool done() const;
private:
    std::list<std::shared_ptr<Action>> m_current;
    std::unordered_map<int, std::shared_ptr<Action>> m_actions;
    std::unordered_map<int, std::vector<int>> m_edges;
    std::unordered_map<long, int> m_inDegree;
    long _nextId;
    std::string m_scriptId;
    bool m_done;
};

inline bool Script::done() const {
    return m_done;
}



class Scheduler : public Runner {
public:
    Scheduler() = default;
    void update(double) override;
//    void start() override;
    void add(std::shared_ptr<Script>);
private:
    std::list<std::shared_ptr<Script>> m_scripts;
    std::unordered_map<std::string, std::shared_ptr<Script>> m_tags;
};