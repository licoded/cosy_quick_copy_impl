#pragma once
#include <vector>
#include "cudd/cuddObj.hh"

class DFA {
    CUDD::Cudd*            mgr_;
    int                    ns_, s_;
    std::vector<bool>      f_;
    std::vector<CUDD::ADD> trans_;
public:
    DFA(CUDD::Cudd& mgr, int ns);

    int  numStates()        const { return ns_; }
    int  start()            const { return s_; }
    bool isAccept(int i)    const { return f_[i]; }
    const CUDD::ADD& trans(int i) const { return trans_[i]; }

    void setStart(int s)              { s_ = s; }
    void setAccept(int i, bool v)                      { f_[i] = v; }
    void setAcceptStates(std::initializer_list<int> states) {
        for (int i : states) f_[i] = true;
    }
    void setTrans(int i, CUDD::ADD t) { trans_[i] = std::move(t); }

    DFA  minimize() const;
    void print()    const;
};
