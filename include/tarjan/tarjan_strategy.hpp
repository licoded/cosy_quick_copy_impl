#pragma once

#include "tarjan_types.hpp"

#include <vector>

namespace tarjan {

// ============================================================================
// ITarjanStrategy - 核心策略接口
// ============================================================================
template <typename Types>
class ITarjanStrategy {
public:
    using Node = typename Types::Node;

    virtual ~ITarjanStrategy() = default;

    // 节点生命周期回调
    virtual void preCheck(Node* node) {}
    virtual void onVisit(Node* node) {}
    virtual void onSccFound(std::vector<Node*>& scc) {}
    virtual void onNodePop(Node* node, Node* prev) {}
    virtual void onRevisit(Node* node, Node* from, bool inPrefix) {}

    // 搜索控制
    virtual bool shouldStop(Node* node) { return false; };
};

} // namespace tarjan
