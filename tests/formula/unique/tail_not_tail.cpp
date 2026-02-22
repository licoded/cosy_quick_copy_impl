#include "formula/formula.hpp"
#include "formula/builder.hpp"
#include "formula/synthesis_context.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

// ============================================================================
// Tail 和 NotTail 唯一性测试
// ============================================================================

TEST_CASE("Unique: !tail == not_tail", "[unique][tail]") {
    SynthesisContext ctx;
    Formula* tail_f = ctx.formula_builder().make_tail();
    Formula* not_tail_f = ctx.formula_builder().make_not_tail();
    Formula* not_tail_expr = ctx.formula_builder().make_unary(Operator::Not, tail_f);

    // !tail 应该等价于 not_tail
    REQUIRE(not_tail_expr == not_tail_f);
}

TEST_CASE("Unique: !not_tail == tail", "[unique][tail]") {
    SynthesisContext ctx;
    Formula* tail_f = ctx.formula_builder().make_tail();
    Formula* not_tail_f = ctx.formula_builder().make_not_tail();
    Formula* not_not_tail = ctx.formula_builder().make_unary(Operator::Not, not_tail_f);

    // !not_tail 应该等价于 tail
    REQUIRE(not_not_tail == tail_f);
}

TEST_CASE("Unique: false R false == tail", "[unique][tail]") {
    SynthesisContext ctx;
    auto& builder = ctx.formula_builder();
    Formula* false_f = builder.make_false();
    Formula* false_r_false = builder.make_binary(Operator::Release, false_f, false_f);
    Formula* tail_f = builder.make_tail();

    // false R false 应该等价于 tail
    REQUIRE(false_r_false == tail_f);
}

TEST_CASE("Unique: true U true == not_tail", "[unique][tail]") {
    SynthesisContext ctx;
    auto& builder = ctx.formula_builder();
    Formula* true_f = builder.make_true();
    Formula* true_u_true = builder.make_binary(Operator::Until, true_f, true_f);
    Formula* not_tail_f = builder.make_not_tail();

    // true U true 应该等价于 not_tail
    REQUIRE(true_u_true == not_tail_f);
}
