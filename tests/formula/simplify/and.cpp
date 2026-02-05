#include "formula/formula.hpp"
#include "formula/synthesis_context.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("Simplify AND: associativity", "[simplify][and]") {
    SynthesisContext ctx;
    Formula* abc1 = ctx.parse_formula("(a & b) & c");
    Formula* abc2 = ctx.parse_formula("a & (b & c)");

    // Before simplify, they are different
    REQUIRE(abc1 != abc2);

    // After simplify with flattening, both should be canonicalized to the same form
    Formula* abc1_simplified = abc1->simplify();
    Formula* abc2_simplified = abc2->simplify();
    REQUIRE(abc1_simplified == abc2_simplified);
}

TEST_CASE("Simplify AND: commutativity with nested structure", "[simplify][and]") {
    SynthesisContext ctx;
    Formula* abc1 = ctx.parse_formula("(a & b) & c");
    Formula* abc2 = ctx.parse_formula("c & (b & a)");

    // With commutativity optimization, nested structures may be canonicalized
    REQUIRE(abc1 == abc2);

    // After simplify with flattening, both should be canonicalized to the same form
    Formula* abc1_simplified = abc1->simplify();
    Formula* abc2_simplified = abc2->simplify();
    REQUIRE(abc1_simplified == abc2_simplified);
}

TEST_CASE("Simplify AND: identity with True", "[simplify][and]") {
    SynthesisContext ctx;
    Formula* a = ctx.parse_formula("a");
    Formula* a_and_true = ctx.parse_formula("a & true");

    // Before simplify, they are different
    REQUIRE(a_and_true != a);

    // After simplify, True is identity for AND: a & true → a
    Formula* simplified = a_and_true->simplify();
    REQUIRE(simplified == a);
}

TEST_CASE("Simplify AND: dominance with False", "[simplify][and]") {
    SynthesisContext ctx;
    Formula* false_f = ctx.parse_formula("false");
    Formula* a_and_false = ctx.parse_formula("a & false");

    // Before simplify, they are different
    REQUIRE(a_and_false != false_f);

    // After simplify, False dominates AND: a & false → false
    Formula* simplified = a_and_false->simplify();
    REQUIRE(simplified == false_f);
}

TEST_CASE("Simplify AND: complementary literals (a & !a)", "[simplify][and][phase3]") {
    SynthesisContext ctx;
    Formula* false_f = ctx.parse_formula("false");
    Formula* a_and_not_a = ctx.parse_formula("a & !a");

    // Before simplify, they are different
    REQUIRE(a_and_not_a != false_f);

    // After simplify, complementary literals detected: a & !a → false
    Formula* simplified = a_and_not_a->simplify();
    REQUIRE(simplified == false_f);
}

TEST_CASE("Simplify AND: complementary literals (!a & a)", "[simplify][and][phase3]") {
    SynthesisContext ctx;
    Formula* false_f = ctx.parse_formula("false");
    Formula* not_a_and_a = ctx.parse_formula("!a & a");

    // Before simplify, they are different
    REQUIRE(not_a_and_a != false_f);

    // After simplify, complementary literals detected: !a & a → false
    Formula* simplified = not_a_and_a->simplify();
    REQUIRE(simplified == false_f);
}

TEST_CASE("Simplify AND: complementary literals with multiple terms", "[simplify][and][phase3]") {
    SynthesisContext ctx;
    Formula* false_f = ctx.parse_formula("false");
    Formula* abc_not_a = ctx.parse_formula("a & b & !a");

    // After simplify, complementary literals detected: a & b & !a → false
    Formula* simplified = abc_not_a->simplify();
    REQUIRE(simplified == false_f);
}

TEST_CASE("Simplify AND: complementary literals in nested structure", "[simplify][and][phase3]") {
    SynthesisContext ctx;
    Formula* false_f = ctx.parse_formula("false");
    Formula* nested = ctx.parse_formula("(a & b) & !a");

    // After simplify with flattening, complementary literals detected: (a & b) & !a → false
    Formula* simplified = nested->simplify();
    REQUIRE(simplified == false_f);
}

TEST_CASE("Simplify AND: nested OR with complementary literals", "[simplify][and][nested]") {
    SynthesisContext ctx;
    Formula* a = ctx.parse_formula("a");
    Formula* input = ctx.parse_formula("a & (c | !c)");

    // Phase 1: collect {a, (c | !c)}
    // Phase 2: (c | !c) simplifies to true, which is skipped
    // Result: a
    Formula* simplified = input->simplify();
    REQUIRE(simplified == a);
}

TEST_CASE("Simplify AND: nested OR with True dominance", "[simplify][and][nested]") {
    SynthesisContext ctx;
    Formula* a = ctx.parse_formula("a");
    Formula* input = ctx.parse_formula("a & (b | true)");

    // Phase 1: collect {a, (b | true)}
    // Phase 2: (b | true) simplifies to true, which is skipped
    // Result: a
    Formula* simplified = input->simplify();
    REQUIRE(simplified == a);
}

TEST_CASE("Simplify AND: nested OR with False identity", "[simplify][and][nested]") {
    SynthesisContext ctx;
    Formula* expected = ctx.parse_formula("a & b");
    Formula* input = ctx.parse_formula("a & (b | false)");

    // Phase 1: collect {a, (b | false)}
    // Phase 2: (b | false) simplifies to b
    // Result: a & b
    Formula* simplified = input->simplify();
    REQUIRE(simplified == expected);
}
