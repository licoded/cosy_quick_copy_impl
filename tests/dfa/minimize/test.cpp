#include <catch2/catch_test_macros.hpp>
#include "dfa/dfa.hpp"

using Cosy::DFA;

TEST_CASE("DFA minimize: G(a -> WNext b)", "[dfa][minimize]") {
    CUDD::Cudd mgr(2);
    CUDD::ADD v0 = mgr.addVar(0);
    CUDD::ADD v1 = mgr.addVar(1);

    auto make_trans = [&](int s_none, int s_a, int s_b, int s_ab) {
        return v1.Ite(v0.Ite(mgr.constant(s_ab), mgr.constant(s_b)),
                      v0.Ite(mgr.constant(s_a),  mgr.constant(s_none)));
    };

    DFA a(mgr, 6);
    a.setStart(0);
    a.setAcceptStates({1, 2, 3, 4});
    a.setTrans(0, make_trans(1, 3, 1, 3));
    a.setTrans(1, make_trans(2, 4, 2, 4));
    a.setTrans(2, make_trans(1, 3, 1, 3));
    a.setTrans(3, make_trans(5, 5, 1, 4));
    a.setTrans(4, make_trans(5, 5, 2, 3));
    a.setTrans(5, make_trans(5, 5, 5, 5));

    REQUIRE(a.numStates() == 6);

    DFA b = a.minimize();

    // q0,q5 各自独立，q1/q2 等价，q3/q4 等价 => 4 个等价类
    REQUIRE(b.numStates() == 4);

    // 起始状态应为 reject
    REQUIRE_FALSE(b.isAccept(b.start()));

    // accept 状态数应为 2（q1/q2 类 和 q3/q4 类）
    int accept_count = 0;
    for (int i = 0; i < b.numStates(); i++)
        if (b.isAccept(i)) ++accept_count;
    REQUIRE(accept_count == 2);
}
