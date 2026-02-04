#include "formula/formula_parser.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("FormulaParser: Unary operator precedence", "[parser][unary]") {
    // Test double negation: ! ! a should be !!a
    Formula* f = new Formula("! ! a");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "!!a");

    // Test Next precedence: X X a should be XXa
    f = new Formula("X X a");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "XXa");

    // Test mixed: ! X a should be !Xa
    f = new Formula("! X a");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "!Xa");

    // Test X ! a should be X!a
    f = new Formula("X ! a");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "X!a");
}