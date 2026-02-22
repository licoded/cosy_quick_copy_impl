#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace tarjan {

// ============================================================================
// DfsContext - DFS遍历上下文
// ============================================================================
class DfsContext {
private:
    std::vector<void*> stack_;
    std::unordered_set<uint64_t> visited_;
    std::unordered_map<uint64_t, int> prefix_idx_;

public:
    void push(void* node, uint64_t id) {
        prefix_idx_[id] = stack_.size();
        visited_.insert(id);
        stack_.push_back(node);
    }

    void* pop() {
        void* node = stack_.back();
        stack_.pop_back();
        return node;
    }

    void* top() const { return stack_.back(); }
    bool empty() const { return stack_.empty(); }
    size_t size() const { return stack_.size(); }

    bool hasVisited(uint64_t id) const { return visited_.count(id) > 0; }
    bool inPrefix(uint64_t id) const { return prefix_idx_.count(id) > 0; }
    void removeFromPrefix(uint64_t id) { prefix_idx_.erase(id); }
    int getPrefixIdx(uint64_t id) const { return prefix_idx_.at(id); }
};

} // namespace tarjan
