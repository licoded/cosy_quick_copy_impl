#include "formula/formula.hpp"
#include "formula/synthesis_context.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("FormulaParser: Unary operator precedence", "[parser][unary]") {
    SynthesisContext ctx;

    // Test double negation: ! ! a should be !!a
    Formula* f = ctx.parse_formula("! ! a");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "!!a");

    // Test Next precedence: X X a should be XXa
    f = ctx.parse_formula("X X a");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "XXa");

    // Test mixed: ! X a should be !Xa
    f = ctx.parse_formula("! X a");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "!Xa");

    // Test X ! a should be X!a
    f = ctx.parse_formula("X ! a");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "X!a");
}