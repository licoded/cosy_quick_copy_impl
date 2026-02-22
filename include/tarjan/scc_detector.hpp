#pragma once

#include "tarjan_state.hpp"

#include <vector>

namespace tarjan {

// ============================================================================
// SccDetector - SCC检测器
// ============================================================================
class SccDetector {
private:
    std::vector<void*> stack_;
    TarjanState* state_;

public:
    explicit SccDetector(TarjanState& state) : state_(&state) {}

    void push(void* node) { stack_.push_back(node); }

    template <typename Node, typename OutputIt>
    OutputIt extractScc(Node* root, OutputIt out) {
        int root_dfn = state_->getDfn(root->getHashId());
        Node* node = nullptr;

        do {
            node = static_cast<Node*>(stack_.back());
            stack_.pop_back();
            state_->setSccRootTime(node->getHashId(), root_dfn);
            *out++ = node;
        } while (state_->getDfn(node->getHashId()) != root_dfn);

        return out;
    }

    bool isRootDetermined(uint64_t id) const {
        return state_->isRootDetermined(id);
    }

    void clear() { stack_.clear(); }
};

} // namespace tarjan
