#pragma once

#include "tarjan_types.hpp"

#include <vector>

namespace tarjan {

// ============================================================================
// ITarjanStrategy - 策略接口（精简版）
// ============================================================================
template <typename Types>
class ITarjanStrategy {
public:
    using Node = typename Types::Node;

    virtual ~ITarjanStrategy() = default;

    // 节点生命周期回调
    virtual void onVisit(Node* node) {}
    virtual void onSccFound(std::vector<Node*>& scc) {}
    virtual void onNodePop(Node* node, Node* prev) {}
    virtual void onRevisit(Node* node, Node* from, bool inPrefix) {}

    // 搜索控制
    virtual bool shouldStop(Node* node) = 0;
    virtual void preCheck(Node* node) {}

    // 可视化（可选）
    virtual void recordNode2VisGraph(Node* node) {}
    virtual void recordTrans2VisGraph(Node* from, Node* to, const void* edge) {}
    virtual void setStateStatus2VisGraph(Node* node) {}
};

} // namespace tarjan
