#include "formula/formula.hpp"
#include "formula/synthesis_context.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("Simplify Next: X[!] False → False", "[simplify][next]") {
    SynthesisContext ctx;
    Formula* false_f = ctx.parse_formula("false");
    Formula* x_false = ctx.parse_formula("X[!] false");

    // Before simplify, they are different
    REQUIRE(x_false != false_f);

    // After simplify: X[!] false → false
    Formula* simplified = x_false->simplify();
    REQUIRE(simplified == false_f);
}

TEST_CASE("Simplify Next: X[!] True (no simplification)", "[simplify][next]") {
    SynthesisContext ctx;
    Formula* true_f = ctx.parse_formula("true");
    Formula* x_true = ctx.parse_formula("X[!] true");

    // X[!] True simplification is disabled, so X[!] true stays as is
    Formula* simplified = x_true->simplify();
    REQUIRE(simplified == x_true);
    REQUIRE(simplified != true_f);
}

TEST_CASE("Simplify Next: X[!] a (no change)", "[simplify][next]") {
    SynthesisContext ctx;
    Formula* x_a = ctx.parse_formula("X[!] a");

    // No simplification for X[!] a
    Formula* simplified = x_a->simplify();
    REQUIRE(simplified == x_a);
}

TEST_CASE("Simplify Next: nested simplification X[!] (a | false)", "[simplify][next]") {
    SynthesisContext ctx;
    Formula* expected = ctx.parse_formula("X[!] a");
    Formula* input = ctx.parse_formula("X[!] (a | false)");

    // Inner (a | false) simplifies to a, then X[!] a
    Formula* simplified = input->simplify();
    REQUIRE(simplified == expected);
}

TEST_CASE("Simplify Next: nested X[!] (a & false) → X[!] false → false", "[simplify][next]") {
    SynthesisContext ctx;
    Formula* false_f = ctx.parse_formula("false");
    Formula* input = ctx.parse_formula("X[!] (a & false)");

    // (a & false) → false, then X[!] false → false
    Formula* simplified = input->simplify();
    REQUIRE(simplified == false_f);
}
