#pragma once

#include "dfs_context.hpp"
#include "edge_iterator.hpp"
#include "scc_detector.hpp"
#include "tarjan_observer.hpp"
#include "tarjan_state.hpp"
#include "tarjan_strategy.hpp"

namespace tarjan {

// ============================================================================
// Tarjan - 主算法类（协调器）
// ============================================================================
template <typename Types>
class Tarjan {
public:
    using Node = typename Types::Node;
    using Edge = typename Types::Edge;

private:
    ITarjanStrategy<Types>* strategy_;
    IEdgeIteratorFactory<Types>* edge_factory_;
    ITarjanObserver<Types>* observer_;

    DfsContext dfs_ctx_;
    TarjanState state_;
    SccDetector scc_detector_;

public:
    Tarjan(ITarjanStrategy<Types>* strategy,
           IEdgeIteratorFactory<Types>* edge_factory,
           ITarjanObserver<Types>* observer = nullptr)
        : strategy_(strategy)
        , edge_factory_(edge_factory)
        , observer_(observer)
        , scc_detector_(state_)
    {
    }

    void dfsSearch(Node* init) {
        if (scc_detector_.isRootDetermined(init->getHashId()))
            return;

        onNewNode(init);

        while (!dfs_ctx_.empty()) {
            Node* cur = static_cast<Node*>(dfs_ctx_.top());
            strategy_->preCheck(cur);

            if (strategy_->shouldStop(cur)) {
                onNodeComplete(cur);
                continue;
            }

            auto iter = edge_factory_->createIterator(cur);
            if (!iter->hasNext()) {
                onNodeComplete(cur);
                continue;
            }

            Edge edge = iter->next();
            Node* next = iter->follow(edge);

            if (observer_) {
                observer_->onTransition(cur, next, edge);
            }

            if (!dfs_ctx_.hasVisited(next->getHashId())) {
                onNewNode(next);
            } else {
                onRevisitNode(cur, next);
            }
        }
    }

    void markNodesAsSearched(const std::vector<Node*>& nodes) {
        for (auto* node : nodes) {
            dfsSearch(node);
        }
    }

    void reset() {
        dfs_ctx_ = DfsContext();
        state_.clear();
        scc_detector_.clear();
    }

private:
    void onNewNode(Node* node) {
        state_.initNode(node->getHashId());
        dfs_ctx_.push(node, node->getHashId());
        scc_detector_.push(node);

        strategy_->preCheck(node);
        strategy_->onVisit(node);

        if (observer_) {
            observer_->onNodeVisited(node);
        }
    }

    void onRevisitNode(Node* cur, Node* next) {
        if (!scc_detector_.isRootDetermined(next->getHashId())) {
            state_.updateLowByDfn(cur->getHashId(), next->getHashId());
        }
        strategy_->onRevisit(next, cur, dfs_ctx_.inPrefix(next->getHashId()));
    }

    void onNodeComplete(Node* node) {
        // 如果是 SCC 根，提取 SCC
        if (state_.isSccRoot(node->getHashId())) {
            std::vector<Node*> scc;
            scc_detector_.extractScc<Node>(node, std::back_inserter(scc));
            strategy_->onSccFound(scc);

            if (observer_) {
                observer_->onSccCompleted(scc);
            }
        }

        // 弹出节点
        dfs_ctx_.pop();
        dfs_ctx_.removeFromPrefix(node->getHashId());

        // 通知策略
        Node* prev = dfs_ctx_.empty() ? nullptr : static_cast<Node*>(dfs_ctx_.top());
        strategy_->onNodePop(node, prev);

        // 更新父节点的 low 值
        if (prev != nullptr) {
            state_.updateLowByLow(prev->getHashId(), node->getHashId());
        }

        if (observer_) {
            observer_->onNodeCompleted(node);
        }
    }
};

} // namespace tarjan
