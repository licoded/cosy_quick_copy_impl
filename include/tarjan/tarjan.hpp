#pragma once

#include "dfs_context.hpp"
#include "edge_iterator.hpp"
#include "scc_detector.hpp"
#include "tarjan_observer.hpp"
#include "tarjan_state.hpp"
#include "tarjan_strategy.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace tarjan {

// ============================================================================
// Tarjan - 主算法类（协调器）
// ============================================================================
template <typename Types>
class Tarjan {
public:
    using Node = typename Types::Node;
    using Edge = typename Types::Edge;
    using Iterator = IEdgeIterator<Types>;
    using IteratorPtr = std::unique_ptr<Iterator>;

private:
    ITarjanStrategy<Types>* strategy_;
    IEdgeIteratorFactory<Types>* edge_factory_;
    std::vector<ITarjanObserver<Types>*> observers_;

    DfsContext dfs_ctx_;
    std::vector<IteratorPtr> iter_stack_;  // 与 dfs_ctx_ 同步的迭代器栈
    TarjanState state_;
    SccDetector scc_detector_;

public:
    Tarjan(ITarjanStrategy<Types>* strategy,
           IEdgeIteratorFactory<Types>* edge_factory)
        : strategy_(strategy)
        , edge_factory_(edge_factory)
        , scc_detector_(state_)
    {
    }

    void addObserver(ITarjanObserver<Types>* observer) {
        if (observer) {
            observers_.push_back(observer);
        }
    }

    void clearObservers() {
        observers_.clear();
    }

    void dfsSearch(Node* init) {
        if (scc_detector_.isRootDetermined(init->getHashId()))
            return;

        pushNode(init);

        while (!dfs_ctx_.empty()) {
            Node* cur = static_cast<Node*>(dfs_ctx_.top());
            Iterator* iter = iter_stack_.back().get();

            strategy_->preCheck(cur);

            if (strategy_->shouldStop(cur) || !iter->hasNext()) {
                popNode();
                continue;
            }

            Edge edge = iter->next();
            Node* next = iter->follow(edge);

            notify(&ITarjanObserver<Types>::onTransition, cur, next, edge);

            if (!dfs_ctx_.hasVisited(next->getHashId())) {
                pushNode(next);
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
        iter_stack_.clear();
        state_.clear();
        scc_detector_.clear();
    }

private:
    template <typename Func, typename... Args>
    void notify(Func func, Args&&... args) {
        for (auto* observer : observers_) {
            (observer->*func)(std::forward<Args>(args)...);
        }
    }

    void pushNode(Node* node) {
        state_.initNode(node->getHashId());
        dfs_ctx_.push(node, node->getHashId());
        iter_stack_.push_back(edge_factory_->createIterator(node));
        scc_detector_.push(node);

        strategy_->preCheck(node);
        strategy_->onVisit(node);
        notify(&ITarjanObserver<Types>::onNodeVisited, node);
    }

    void popNode() {
        Node* node = static_cast<Node*>(dfs_ctx_.pop());
        iter_stack_.pop_back();

        // 如果是 SCC 根，提取 SCC
        if (state_.isSccRoot(node->getHashId())) {
            std::vector<Node*> scc;
            scc_detector_.extractScc<Node>(node, std::back_inserter(scc));
            strategy_->onSccFound(scc);
            notify(&ITarjanObserver<Types>::onSccCompleted, scc);
        }

        dfs_ctx_.removeFromPrefix(node->getHashId());

        // 通知策略
        Node* prev = dfs_ctx_.empty() ? nullptr : static_cast<Node*>(dfs_ctx_.top());
        strategy_->onNodePop(node, prev);

        // 更新父节点的 low 值
        if (prev != nullptr) {
            state_.updateLowByLow(prev->getHashId(), node->getHashId());
        }

        notify(&ITarjanObserver<Types>::onNodeCompleted, node);
    }

    void onRevisitNode(Node* cur, Node* next) {
        if (!scc_detector_.isRootDetermined(next->getHashId())) {
            state_.updateLowByDfn(cur->getHashId(), next->getHashId());
        }
        strategy_->onRevisit(next, cur, dfs_ctx_.inPrefix(next->getHashId()));
    }
};

} // namespace tarjan
