#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

namespace Cosy {

template<typename Label>
DFA<Label> DFA<Label>::minimize() const {
    int num_old = 0, num_new = 1; // 保证第一次能进入循环
    std::vector<CUDD::ADD> remapped(ns_, mgr_->addZero());
    std::vector<int> discrs(ns_, 0);

    while (num_new > num_old) {
        std::unordered_map<CUDD::ADD, CUDD::ADD, dfa_detail::AddHash> memo;
        for (int i = 0; i < ns_; i++)
            remapped[i] = dfa_detail::remap_rec(*mgr_, trans_[i], discrs.data(), memo);

        // 根据 remapped 划分状态，状态 i 的划分由 (extra_[i], f_[i], remapped[i]) 决定
        std::unordered_map<dfa_detail::Key<Label>, int, dfa_detail::KH<Label>> kmap;
        num_old = num_new; num_new = 0;
        for (int i = 0; i < ns_; i++) {
            auto [it, ins] = kmap.emplace(dfa_detail::Key<Label>{extra_[i], f_[i], remapped[i].getNode()}, num_new);
            discrs[i] = it->second;
            if (ins) ++num_new;
        }

        spdlog::trace("discrs: {}  ({} -> {})", fmt::join(discrs, " "), num_old, num_new);
    }

    DFA b(*mgr_, num_new);
    b.s_ = discrs[s_];
    std::vector<bool> assigned(num_new, false);
    for (int i = 0; i < ns_; i++) {
        int c = discrs[i];
        b.f_[c] = f_[i];
        b.extra_[c] = extra_[i];
        if (!assigned[c]) { b.trans_[c] = remapped[i]; assigned[c] = true; }
    }
    return b;
}

template<typename Label>
void DFA<Label>::print() const {
    spdlog::info("DFA: {} states, start={}", ns_, s_);
    for (int i = 0; i < ns_; i++)
        spdlog::info("  state {}: {}  trans_nodes={}", i, f_[i] ? "accept" : "reject", trans_[i].nodeCount());
}

} // namespace Cosy
