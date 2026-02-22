#pragma once

#include "tarjan/tarjan.hpp"

#include <algorithm>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

namespace simple_graph {

// ============================================================================
// 节点和边定义
// ============================================================================
struct Edge {
    int to;
};

struct Node {
    int id;
    std::vector<Edge> edges;

    uint64_t getHashId() const { return static_cast<uint64_t>(id); }
};

using Types = tarjan::TarjanTypes<Node, Edge>;

// ============================================================================
// GraphBuilder - 图构建器（前置声明）
// ============================================================================
class GraphBuilder;

// ============================================================================
// EdgeIterator - 边迭代器
// ============================================================================
class EdgeIterator : public tarjan::IEdgeIterator<Types> {
private:
    GraphBuilder* graph_;
    Node* node_;
    size_t index_;

public:
    EdgeIterator(GraphBuilder* graph, Node* node)
        : graph_(graph), node_(node), index_(0) {}

    bool hasNext() override { return index_ < node_->edges.size(); }

    Edge next() override { return node_->edges[index_++]; }

    Node* follow(const Edge& edge) override;
};

// ============================================================================
// EdgeIteratorFactory - 边迭代器工厂
// ============================================================================
class EdgeIteratorFactory : public tarjan::IEdgeIteratorFactory<Types> {
private:
    GraphBuilder* graph_;

public:
    explicit EdgeIteratorFactory(GraphBuilder* graph) : graph_(graph) {}

    std::unique_ptr<tarjan::IEdgeIterator<Types>> createIterator(Node* node) override {
        return std::make_unique<EdgeIterator>(graph_, node);
    }
};

// ============================================================================
// SccCollector - SCC 收集策略
// ============================================================================
class SccCollector : public tarjan::ITarjanStrategy<Types> {
public:
    std::vector<std::vector<int>> sccs;

    void onSccFound(std::vector<Node*>& scc) override {
        std::vector<int> sccIds;
        for (auto* node : scc) {
            sccIds.push_back(node->id);
        }
        std::sort(sccIds.begin(), sccIds.end());
        sccs.push_back(sccIds);
    }
};

// ============================================================================
// GraphBuilder - 图构建器
// ============================================================================
class GraphBuilder {
public:
    std::vector<Node> nodes;
    EdgeIteratorFactory factory;
    SccCollector collector;

    GraphBuilder() : factory(this) {}

    void addNodeWithEdges(int id, std::vector<int> edgesTo) {
        if (nodeIdToIndex_.count(id)) {
            throw std::runtime_error("Duplicate node id: " + std::to_string(id));
        }

        std::vector<Edge> edges;
        for (int to : edgesTo) {
            edges.push_back({to});
        }

        nodeIdToIndex_[id] = nodes.size();
        nodes.push_back({id, edges});
    }

    Node* getNodeById(int id) {
        auto it = nodeIdToIndex_.find(id);
        if (it == nodeIdToIndex_.end()) {
            return nullptr;
        }
        return &nodes[it->second];
    }

    std::set<std::set<int>> getSccsAsSets() {
        std::set<std::set<int>> result;
        for (const auto& scc : collector.sccs) {
            result.insert(std::set<int>(scc.begin(), scc.end()));
        }
        return result;
    }

    void clear() {
        nodes.clear();
        nodeIdToIndex_.clear();
        collector.sccs.clear();
    }

private:
    std::map<int, size_t> nodeIdToIndex_;
};

// ============================================================================
// EdgeIterator::follow 实现（需要 GraphBuilder 完整定义）
// ============================================================================
inline Node* EdgeIterator::follow(const Edge& edge) {
    return graph_->getNodeById(edge.to);
}

} // namespace simple_graph
