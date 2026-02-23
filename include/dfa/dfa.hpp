#pragma once
#include <vector>
#include "cudd/cuddObj.hh"
#include "dfa/dfa_detail.hpp"

namespace Cosy {

struct NoLabel {
    bool operator==(const NoLabel&) const { return true; }
    size_t hashId() const { return 0; }
};

template<typename Label = NoLabel>
class DFA {
    CUDD::Cudd*            mgr_;
    int                    ns_, s_;
    std::vector<bool>      f_;
    std::vector<CUDD::ADD> trans_;
    std::vector<Label>     extra_;
public:
    DFA(CUDD::Cudd& m, int n)
        : mgr_(&m), ns_(n), s_(0), f_(n, false), trans_(n, m.addZero()), extra_(n) {}

    int  numStates()          const { return ns_; }
    int  start()              const { return s_; }
    bool isAccept(int i)      const { return f_[i]; }
    const CUDD::ADD& trans(int i) const { return trans_[i]; }
    const Label& label(int i) const { return extra_[i]; }

    void setStart(int s)                                    { s_ = s; }
    void setAccept(int i, bool v)                           { f_[i] = v; }
    void setAcceptStates(std::initializer_list<int> states) { for (int i : states) f_[i] = true; }
    void setTrans(int i, CUDD::ADD t)                       { trans_[i] = std::move(t); }
    void setLabel(int i, Label v)                           { extra_[i] = std::move(v); }

    DFA minimize() const;
    void print()   const;
};

} // namespace Cosy

#include "dfa/dfa.tpp"
