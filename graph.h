#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

template<typename T>
class Graph {
public:
    Graph() : m_next(0) {}

    int addNode (T value) {
        m_nodes[m_next] = value;
        return m_next++;
    }


    void removeNode(int key) {
        m_nodes.erase(key);
        // check all edges from key
        auto it = m_edges.find(key);
        if (it != m_edges.end()) {
            for (const auto& edge : it->second) {
                m_edges.at(edge.first).erase(key);
            }
            m_edges.erase(key);
        }
        if (key == m_next - 1) {
            m_next--;
        }

    }

    void addEdge (int tail, int head, float weight) {
        m_edges[tail].insert(std::make_pair(head, weight));
        m_edges[head].insert(std::make_pair(tail, weight));
    }

    void removeEdge (int tail, int head) {
        m_edges[tail].erase(head);
        m_edges[head].erase(tail);
    }

    float shortestPath(int start, int end, std::vector<int>& path) {
        // we'll mark every node with its minimum distance to node C (our selected node).
        // For start node, this distance is 0. For the rest of nodes, as we still don't know that minimum distance,
        // it starts being infinity (∞)
        std::unordered_map<int, float> dist;
        std::unordered_set<int> visited;
        std::unordered_map<int, int> next;

        for (const auto& node : m_nodes) {
            dist[node.first] = node.first == start ? 0 : std::numeric_limits<float>::infinity();
        }
        int currentNode = start;
        bool done = false;
        while (!done) {
            auto it = m_edges.find(currentNode);
            if (it != m_edges.end()) {
                float currentDistance = dist.at(currentNode);
                for (const auto& edge : it->second) {
                    if (visited.count(edge.first) == 0) {
                        auto ndist = dist.at(edge.first);
                        if (currentDistance + edge.second < ndist) {
                            // update
                            dist[edge.first] = currentDistance + edge.second;
                            next[edge.first] = currentNode;
                        }
                    }
                }
            }
            visited.insert(currentNode);
            // update the current node. It's the unvisited node with the smallest minimum distance
            float minDist = std::numeric_limits<float>::infinity();
            done = true;
            for (const auto& node : m_nodes) {
                if (visited.count(node.first) == 0 && dist.at(node.first) < minDist) {
                    currentNode = node.first;
                    minDist = dist.at(node.first);
                    done = false;
                }
            }
        }
        // fill up keys
        auto node = end;
        while (true) {
            path.push_back(node);
            auto it = next.find(node);
            if (it == next.end()) {
                break;
            } else {
                node = next.at(node);
            }
        }
        return dist.at(end);
    }

    size_t getVertexCount() const {
        return m_nodes.size();
    }

    size_t getEdgeCount() const {
        return m_edges.size();
    }

    std::unordered_map<int, T>& getNodes() {
        return m_nodes;
    }

    const T& getNode(int id) {
        return m_nodes[id];
    }

    std::unordered_map<int, std::unordered_map<int, float>>& getEdges() {
        return m_edges;
    }
private:
    std::unordered_map<int, std::unordered_map<int, float>> m_edges;
    std::unordered_map<int, T> m_nodes;
    int m_next;
};