#include "formula/formula.hpp"
#include "formula/synthesis_context.hpp"
#include "formula/simplify.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("HashConsing: True singleton", "[hash_consing]") {
    SynthesisContext ctx;
    Formula* t1 = ctx.parse_formula("true");
    Formula* t2 = ctx.parse_formula("true");

    REQUIRE(t1 == t2);
    REQUIRE(t1->toString() == "true");
}

TEST_CASE("HashConsing: False singleton", "[hash_consing]") {
    SynthesisContext ctx;
    Formula* f1 = ctx.parse_formula("false");
    Formula* f2 = ctx.parse_formula("false");

    REQUIRE(f1 == f2);
    REQUIRE(f1->toString() == "false");
}

TEST_CASE("HashConsing: Literal deduplication", "[hash_consing]") {
    SynthesisContext ctx;
    Formula* a1 = ctx.parse_formula("a");
    Formula* a2 = ctx.parse_formula("a");

    REQUIRE(a1 == a2);
    REQUIRE(a1->toString() == "a");
}

TEST_CASE("HashConsing: Structural deduplication", "[hash_consing]") {
    SynthesisContext ctx;
    Formula* ab1 = ctx.parse_formula("a & b");
    Formula* ab2 = ctx.parse_formula("a & b");

    REQUIRE(ab1 == ab2);
}

TEST_CASE("HashConsing: AND commutativity", "[hash_consing][canonicalization]") {
    SynthesisContext ctx;
    Formula* ab = ctx.parse_formula("a & b");
    Formula* ba = ctx.parse_formula("b & a");

    // Before simplify, they are different
    REQUIRE(ab != ba);

    // After simplify, they should be canonicalized to the same formula
    Formula* ab_simplified = simplify(ab, ctx.formula_builder());
    Formula* ba_simplified = simplify(ba, ctx.formula_builder());
    REQUIRE(ab_simplified == ba_simplified);
}

TEST_CASE("HashConsing: OR commutativity", "[hash_consing][canonicalization]") {
    SynthesisContext ctx;
    Formula* ab = ctx.parse_formula("a | b");
    Formula* ba = ctx.parse_formula("b | a");

    // Before simplify, they are different
    REQUIRE(ab != ba);

    // After simplify, they should be canonicalized to the same formula
    Formula* ab_simplified = simplify(ab, ctx.formula_builder());
    Formula* ba_simplified = simplify(ba, ctx.formula_builder());
    REQUIRE(ab_simplified == ba_simplified);
}

TEST_CASE("HashConsing: AND associativity", "[hash_consing][canonicalization]") {
    SynthesisContext ctx;
    Formula* abc1 = ctx.parse_formula("(a & b) & c");
    Formula* abc2 = ctx.parse_formula("a & (b & c)");

    // Before simplify, they are different
    REQUIRE(abc1 != abc2);

    // After simplify with flattening, both should be canonicalized to the same form
    Formula* abc1_simplified = simplify(abc1, ctx.formula_builder());
    Formula* abc2_simplified = simplify(abc2, ctx.formula_builder());
    REQUIRE(abc1_simplified == abc2_simplified);
}

TEST_CASE("HashConsing: OR associativity", "[hash_consing][canonicalization]") {
    SynthesisContext ctx;
    Formula* abc1 = ctx.parse_formula("(a | b) | c");
    Formula* abc2 = ctx.parse_formula("a | (b | c)");

    // Before simplify, they are different
    REQUIRE(abc1 != abc2);

    // After simplify with flattening, both should be canonicalized to the same form
    Formula* abc1_simplified = simplify(abc1, ctx.formula_builder());
    Formula* abc2_simplified = simplify(abc2, ctx.formula_builder());
    REQUIRE(abc1_simplified == abc2_simplified);
}

TEST_CASE("HashConsing: AND with True (identity)", "[hash_consing][simplification]") {
    SynthesisContext ctx;
    Formula* a = ctx.parse_formula("a");
    Formula* a_and_true = ctx.parse_formula("a & true");

    // Before simplify, they are different
    REQUIRE(a_and_true != a);

    // After simplify, True is identity for AND: a & true → a
    Formula* simplified = simplify(a_and_true, ctx.formula_builder());
    REQUIRE(simplified == a);
}

TEST_CASE("HashConsing: AND with False (dominance)", "[hash_consing][simplification]") {
    SynthesisContext ctx;
    Formula* false_f = ctx.parse_formula("false");
    Formula* a_and_false = ctx.parse_formula("a & false");

    // Before simplify, they are different
    REQUIRE(a_and_false != false_f);

    // After simplify, False dominates AND: a & false → false
    Formula* simplified = simplify(a_and_false, ctx.formula_builder());
    REQUIRE(simplified == false_f);
}

TEST_CASE("HashConsing: OR with False (identity)", "[hash_consing][simplification]") {
    SynthesisContext ctx;
    Formula* a = ctx.parse_formula("a");
    Formula* a_or_false = ctx.parse_formula("a | false");

    // Before simplify, they are different
    REQUIRE(a_or_false != a);

    // After simplify, False is identity for OR: a | false → a
    Formula* simplified = simplify(a_or_false, ctx.formula_builder());
    REQUIRE(simplified == a);
}

TEST_CASE("HashConsing: OR with True (dominance)", "[hash_consing][simplification]") {
    SynthesisContext ctx;
    Formula* true_f = ctx.parse_formula("true");
    Formula* a_or_true = ctx.parse_formula("a | true");

    // Before simplify, they are different
    REQUIRE(a_or_true != true_f);

    // After simplify, True dominates OR: a | true → true
    Formula* simplified = simplify(a_or_true, ctx.formula_builder());
    REQUIRE(simplified == true_f);
}

