#pragma once

#include <algorithm>
#include <unordered_map>

namespace tarjan {

// ============================================================================
// TarjanState - Tarjan算法状态
// ============================================================================
class TarjanState {
private:
    std::unordered_map<uint64_t, int> dfn_;
    std::unordered_map<uint64_t, int> low_;
    std::unordered_map<uint64_t, int> scc_root_time_;
    int time_ = 0;

public:
    void initNode(uint64_t id) {
        dfn_[id] = low_[id] = time_++;
        scc_root_time_[id] = -1;
    }

    void updateLowByDfn(uint64_t cur, uint64_t next) {
        low_[cur] = std::min(low_[cur], dfn_[next]);
    }

    void updateLowByLow(uint64_t cur, uint64_t next) {
        low_[cur] = std::min(low_[cur], low_[next]);
    }

    bool isSccRoot(uint64_t id) const { return low_.at(id) == dfn_.at(id); }

    bool isRootDetermined(uint64_t id) const {
        auto it = scc_root_time_.find(id);
        return it != scc_root_time_.end() && it->second != -1;
    }

    void setSccRootTime(uint64_t id, int time) { scc_root_time_[id] = time; }
    int getDfn(uint64_t id) const { return dfn_.at(id); }
    int getLow(uint64_t id) const { return low_.at(id); }

    void clear() {
        dfn_.clear();
        low_.clear();
        scc_root_time_.clear();
        time_ = 0;
    }
};

} // namespace tarjan
