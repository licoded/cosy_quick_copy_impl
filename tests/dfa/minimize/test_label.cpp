#include <catch2/catch_test_macros.hpp>
#include <spdlog/spdlog.h>
#include <stack>
#include <unordered_set>
#include "dfa/dfa.hpp"
#include "formula/formula.hpp"
#include "formula/synthesis_context.hpp"

using Cosy::DFA;
using Cosy::Formula;
using Cosy::SynthesisContext;

static void assign_labels(DFA<Formula*>& dfa, CUDD::Cudd& mgr,
                          Formula* init, int id_a, int id_b) {
    std::vector<bool> visited(dfa.numStates(), false);
    dfa.setLabel(dfa.start(), init);
    visited[dfa.start()] = true;

    std::stack<int> stk;
    stk.push(dfa.start());
    while (!stk.empty()) {
        int s = stk.top(); stk.pop();
        Formula* f = dfa.label(s);
        for (int va = 0; va <= 1; va++) {
            for (int vb = 0; vb <= 1; vb++) {
                int inputs[2] = {va, vb};
                int s2 = (int)Cudd_V(Cudd_Eval(mgr.getManager(), dfa.trans(s).getNode(), inputs));
                if (visited[s2]) continue;
                visited[s2] = true;
                std::unordered_set<int> lits = {va ? id_a : -id_a, vb ? id_b : -id_b};
                dfa.setLabel(s2, f->nnf()->xnf()->progression(lits));
                stk.push(s2);
            }
        }
    }
}

static void print_labels(const DFA<Formula*>& dfa) {
    for (int i = 0; i < dfa.numStates(); i++)
        spdlog::info("  state {}: {}", i,
                     dfa.label(i) ? dfa.label(i)->toString() : "(none)");
}

TEST_CASE("DFA with LTLf formula labels: G(a -> X b)", "[dfa][label]") {
    CUDD::Cudd mgr(2);
    CUDD::ADD v0 = mgr.addVar(0);
    CUDD::ADD v1 = mgr.addVar(1);

    auto make_trans = [&](int s_none, int s_a, int s_b, int s_ab) {
        return v1.Ite(v0.Ite(mgr.constant(s_ab), mgr.constant(s_b)),
                      v0.Ite(mgr.constant(s_a),  mgr.constant(s_none)));
    };

    DFA<Formula*> a(mgr, 6);
    a.setStart(0);
    a.setAcceptStates({1, 2, 3, 4});
    a.setTrans(0, make_trans(1, 3, 1, 3));
    a.setTrans(1, make_trans(2, 4, 2, 4));
    a.setTrans(2, make_trans(1, 3, 1, 3));
    a.setTrans(3, make_trans(5, 5, 1, 4));
    a.setTrans(4, make_trans(5, 5, 2, 3));
    a.setTrans(5, make_trans(5, 5, 5, 5));

    SynthesisContext ctx;
    Formula* init = ctx.parse_formula("G(a -> X b)");
    int id_a = ctx.symbols().get_or_create_variable_id("a");
    int id_b = ctx.symbols().get_or_create_variable_id("b");

    assign_labels(a, mgr, init, id_a, id_b);
    spdlog::info("Original DFA labels:");
    print_labels(a);

    DFA<Formula*> b = a.minimize();
    assign_labels(b, mgr, init, id_a, id_b);
    spdlog::info("Minimized DFA labels:");
    print_labels(b);

    REQUIRE(b.numStates() == 4);
}
