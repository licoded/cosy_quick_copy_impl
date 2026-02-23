#pragma once
#include <unordered_map>
#include "cudd/cuddObj.hh"

namespace Cosy {
namespace dfa_detail {

struct AddHash {
    size_t operator()(const CUDD::ADD& a) const {
        return std::hash<uintptr_t>()((uintptr_t)a.getNode());
    }
};

inline CUDD::ADD add_node(CUDD::Cudd& mgr, int var, CUDD::ADD t, CUDD::ADD e) {
    return mgr.addVar(var).Ite(t, e);
}

inline CUDD::ADD remap_rec(CUDD::Cudd& mgr, CUDD::ADD n, const int* d,
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

template<typename Label>
using Key = std::tuple<Label, bool, DdNode*>;

template<typename Label>
struct KH {
    size_t operator()(const Key<Label>& k) const {
        size_t h = std::hash<Label>()(std::get<0>(k));
        h ^= std::hash<bool>()(std::get<1>(k)) << 1;
        h ^= std::hash<uintptr_t>()((uintptr_t)std::get<2>(k)) << 2;
        return h;
    }
};

} // namespace dfa_detail
} // namespace Cosy
