#pragma once

#include "tarjan_types.hpp"

#include <vector>

namespace tarjan {

// ============================================================================
// ITarjanObserver - 观察者接口（可视化/日志等）
// ============================================================================
template <typename Types>
class ITarjanObserver {
public:
    using Node = typename Types::Node;
    using Edge = typename Types::Edge;

    virtual ~ITarjanObserver() = default;

    // 节点事件
    virtual void onNodeVisited(Node* node) {}
    virtual void onNodeCompleted(Node* node) {}

    // 边事件
    virtual void onTransition(Node* from, Node* to, const Edge& edge) {}

    // SCC 事件
    virtual void onSccCompleted(const std::vector<Node*>& scc) {}
};

} // namespace tarjan
