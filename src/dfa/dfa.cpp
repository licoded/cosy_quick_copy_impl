#include "dfa/dfa.hpp"
#include <cstdio>
#include <unordered_map>

DFA::DFA(CUDD::Cudd& m, int n)
    : mgr_(&m), ns_(n), s_(0), f_(n, false), trans_(n, m.addZero()) {}

struct AddHash {
    size_t operator()(const CUDD::ADD& a) const {
        return std::hash<uintptr_t>()((uintptr_t)a.getNode());
    }
};

static CUDD::ADD add_node(CUDD::Cudd& mgr, int var, CUDD::ADD t, CUDD::ADD e) {
    return mgr.addVar(var).Ite(t, e);
}

static CUDD::ADD remap_rec(CUDD::Cudd& mgr, CUDD::ADD n, const int* d,
                           std::unordered_map<CUDD::ADD, CUDD::ADD, AddHash>& memo) {
    DdNode* node = n.getNode();
    if (Cudd_IsConstant(node)) // NOTE: 不要加缓存 因为已经是常数了 加缓存反而会增加开销
        return mgr.constant(d[(int)Cudd_V(node)]);
    auto it = memo.find(n);
    if (it != memo.end()) return it->second;
    CUDD::ADD t = remap_rec(mgr, CUDD::ADD(mgr, Cudd_T(node)), d, memo);
    CUDD::ADD e = remap_rec(mgr, CUDD::ADD(mgr, Cudd_E(node)), d, memo);
    CUDD::ADD r = add_node(mgr, n.NodeReadIndex(), t, e);
    return memo.emplace(n, r).first->second;
}

using Key = std::pair<bool, DdNode*>;
struct KH {
    size_t operator()(const Key& k) const {
        return std::hash<bool>()(k.first) ^
                (std::hash<uintptr_t>()((uintptr_t)k.second) << 1);
    }
};

DFA DFA::minimize() const {
    int num_old = 0, num_new = 1; // 保证第一次能进入循环
    std::vector<CUDD::ADD> remapped(ns_, mgr_->addZero());
    std::vector<int> discrs(ns_, 0);

    while (num_new > num_old) {
        std::unordered_map<CUDD::ADD, CUDD::ADD, AddHash> memo;
        for (int i = 0; i < ns_; i++)
            remapped[i] = remap_rec(*mgr_, trans_[i], discrs.data(), memo);

        // 根据 remapped 划分状态，状态 i 的划分由 (f_[i], remapped[i]) 决定
        std::unordered_map<Key, int, KH> kmap;
        num_old = num_new; num_new = 0;
        for (int i = 0; i < ns_; i++) {
            auto [it, ins] = kmap.emplace(Key{f_[i], remapped[i].getNode()}, num_new);
            discrs[i] = it->second;
            if (ins) ++num_new;
        }

        // print discrs
        printf("discrs: ");
        for (int i = 0; i < ns_; i++) printf("%d ", discrs[i]);
        printf("\n");
    }

    DFA b(*mgr_, num_new);
    b.s_ = discrs[s_];
    std::vector<bool> assigned(num_new, false);
    for (int i = 0; i < ns_; i++) {
        int c = discrs[i];
        b.f_[c] = f_[i];
        if (!assigned[c]) { b.trans_[c] = remapped[i]; assigned[c] = true; }
    }
    return b;
}

void DFA::print() const {
    printf("DFA: %d states, start=%d\n", ns_, s_);
    for (int i = 0; i < ns_; i++)
        printf("  state %d: %s  trans_nodes=%d\n",
               i, f_[i] ? "accept" : "reject", trans_[i].nodeCount());
}
