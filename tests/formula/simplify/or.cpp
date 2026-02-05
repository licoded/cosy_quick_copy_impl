#include "formula/formula.hpp"
#include "formula/synthesis_context.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("Simplify OR: associativity", "[simplify][or]") {
    SynthesisContext ctx;
    Formula* abc1 = ctx.parse_formula("(a | b) | c");
    Formula* abc2 = ctx.parse_formula("a | (b | c)");

    // Before simplify, they are different
    REQUIRE(abc1 != abc2);

    // After simplify with flattening, both should be canonicalized to the same form
    Formula* abc1_simplified = abc1->simplify();
    Formula* abc2_simplified = abc2->simplify();
    REQUIRE(abc1_simplified == abc2_simplified);
}

TEST_CASE("Simplify OR: identity with False", "[simplify][or]") {
    SynthesisContext ctx;
    Formula* a = ctx.parse_formula("a");
    Formula* a_or_false = ctx.parse_formula("a | false");

    // Before simplify, they are different
    REQUIRE(a_or_false != a);

    // After simplify, False is identity for OR: a | false → a
    Formula* simplified = a_or_false->simplify();
    REQUIRE(simplified == a);
}

TEST_CASE("Simplify OR: dominance with True", "[simplify][or]") {
    SynthesisContext ctx;
    Formula* true_f = ctx.parse_formula("true");
    Formula* a_or_true = ctx.parse_formula("a | true");

    // Before simplify, they are different
    REQUIRE(a_or_true != true_f);

    // After simplify, True dominates OR: a | true → true
    Formula* simplified = a_or_true->simplify();
    REQUIRE(simplified == true_f);
}

TEST_CASE("Simplify OR: complementary literals (a | !a)", "[simplify][or][phase3]") {
    SynthesisContext ctx;
    Formula* true_f = ctx.parse_formula("true");
    Formula* a_or_not_a = ctx.parse_formula("a | !a");

    // Before simplify, they are different
    REQUIRE(a_or_not_a != true_f);

    // After simplify, complementary literals detected: a | !a → true
    Formula* simplified = a_or_not_a->simplify();
    REQUIRE(simplified == true_f);
}

TEST_CASE("Simplify OR: complementary literals (!a | a)", "[simplify][or][phase3]") {
    SynthesisContext ctx;
    Formula* true_f = ctx.parse_formula("true");
    Formula* not_a_or_a = ctx.parse_formula("!a | a");

    // Before simplify, they are different
    REQUIRE(not_a_or_a != true_f);

    // After simplify, complementary literals detected: !a | a → true
    Formula* simplified = not_a_or_a->simplify();
    REQUIRE(simplified == true_f);
}

TEST_CASE("Simplify OR: complementary literals with multiple terms", "[simplify][or][phase3]") {
    SynthesisContext ctx;
    Formula* true_f = ctx.parse_formula("true");
    Formula* abc_not_a = ctx.parse_formula("a | b | !a");

    // After simplify, complementary literals detected: a | b | !a → true
    Formula* simplified = abc_not_a->simplify();
    REQUIRE(simplified == true_f);
}

TEST_CASE("Simplify OR: complementary literals in nested structure", "[simplify][or][phase3]") {
    SynthesisContext ctx;
    Formula* true_f = ctx.parse_formula("true");
    Formula* nested = ctx.parse_formula("(a | b) | !a");

    // After simplify with flattening, complementary literals detected: (a | b) | !a → true
    Formula* simplified = nested->simplify();
    REQUIRE(simplified == true_f);
}
