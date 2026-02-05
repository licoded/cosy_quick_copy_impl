#include "formula/formula.hpp"
#include "formula/synthesis_context.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("Simplify WNext: X True → True", "[simplify][wnext]") {
    SynthesisContext ctx;
    Formula* true_f = ctx.parse_formula("true");
    Formula* wx_true = ctx.parse_formula("X true");

    // Before simplify, they are different
    REQUIRE(wx_true != true_f);

    // After simplify: X true → true
    Formula* simplified = wx_true->simplify();
    REQUIRE(simplified == true_f);
}

TEST_CASE("Simplify WNext: X false (no simplification)", "[simplify][wnext]") {
    SynthesisContext ctx;
    Formula* false_f = ctx.parse_formula("false");
    Formula* wx_false = ctx.parse_formula("X false");

    // X False simplification is disabled, so X false stays as is
    Formula* simplified = wx_false->simplify();
    REQUIRE(simplified == wx_false);
    REQUIRE(simplified != false_f);
}

TEST_CASE("Simplify WNext: X a (no change)", "[simplify][wnext]") {
    SynthesisContext ctx;
    Formula* wx_a = ctx.parse_formula("X a");

    // No simplification for X a
    Formula* simplified = wx_a->simplify();
    REQUIRE(simplified == wx_a);
}

TEST_CASE("Simplify WNext: nested simplification X (a | false)", "[simplify][wnext]") {
    SynthesisContext ctx;
    Formula* expected = ctx.parse_formula("X a");
    Formula* input = ctx.parse_formula("X (a | false)");

    // Inner (a | false) simplifies to a, then X a
    Formula* simplified = input->simplify();
    REQUIRE(simplified == expected);
}

TEST_CASE("Simplify WNext: nested X (a | true) → X true → true", "[simplify][wnext]") {
    SynthesisContext ctx;
    Formula* true_f = ctx.parse_formula("true");
    Formula* input = ctx.parse_formula("X (a | true)");

    // (a | true) → true, then X true → true
    Formula* simplified = input->simplify();
    REQUIRE(simplified == true_f);
}
