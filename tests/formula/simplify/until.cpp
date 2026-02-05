#include "formula/formula.hpp"
#include "formula/synthesis_context.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("Simplify Until: False U a → a", "[simplify][until]") {
    SynthesisContext ctx;
    Formula* a = ctx.parse_formula("a");
    Formula* false_until_a = ctx.parse_formula("false U a");

    // False U a should simplify to a
    Formula* simplified = false_until_a->simplify();
    REQUIRE(simplified == a);
}

TEST_CASE("Simplify Until: a U False → False", "[simplify][until]") {
    SynthesisContext ctx;
    Formula* false_f = ctx.parse_formula("false");
    Formula* a_until_false = ctx.parse_formula("a U false");

    // a U False should simplify to False
    Formula* simplified = a_until_false->simplify();
    REQUIRE(simplified == false_f);
}

TEST_CASE("Simplify Until: True U a → True", "[simplify][until]") {
    SynthesisContext ctx;
    Formula* true_f = ctx.parse_formula("true");
    Formula* true_until_a = ctx.parse_formula("true U a");

    // True U a should simplify to True
    Formula* simplified = true_until_a->simplify();
    REQUIRE(simplified == true_f);
}

TEST_CASE("Simplify Until: a U True → True", "[simplify][until]") {
    SynthesisContext ctx;
    Formula* true_f = ctx.parse_formula("true");
    Formula* a_until_true = ctx.parse_formula("a U true");

    // a U True should simplify to True
    Formula* simplified = a_until_true->simplify();
    REQUIRE(simplified == true_f);
}

TEST_CASE("Simplify Until: a U a → a", "[simplify][until]") {
    SynthesisContext ctx;
    Formula* a = ctx.parse_formula("a");
    Formula* a_until_a = ctx.parse_formula("a U a");

    // a U a should simplify to a
    Formula* simplified = a_until_a->simplify();
    REQUIRE(simplified == a);
}

TEST_CASE("Simplify Until: nested (false U a) U b → a U b", "[simplify][until]") {
    SynthesisContext ctx;
    Formula* a = ctx.parse_formula("a");
    Formula* b = ctx.parse_formula("b");
    Formula* nested = ctx.parse_formula("(false U a) U b");  // should become (a U b)

    Formula* simplified = nested->simplify();
    Formula* expected = ctx.parse_formula("a U b");
    REQUIRE(simplified == expected);
}

TEST_CASE("Simplify Until: nested a U (b U false) → a U false", "[simplify][until]") {
    SynthesisContext ctx;
    Formula* a = ctx.parse_formula("a");
    Formula* nested = ctx.parse_formula("a U (b U false)");  // should become (a U false), which simplifies to false

    Formula* simplified = nested->simplify();
    Formula* expected = ctx.parse_formula("false");
    REQUIRE(simplified == expected);
}