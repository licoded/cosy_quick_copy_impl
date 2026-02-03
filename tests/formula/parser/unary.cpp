#include "formula/formula_parser.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("FormulaParser: Unary operator precedence", "[parser][unary]") {
    Formula parser;

    // Test double negation: ! ! a should be !(!(a))
    auto f = parser.parse("! ! a");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "!(!(a))");
    REQUIRE(parser.error().empty());

    // Test Next precedence: X X a should be X(X(a))
    f = parser.parse("X X a");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "X(X(a))");

    // Test mixed: ! X a should be !(X(a))
    f = parser.parse("! X a");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "!(X(a))");

    // Test X ! a should be X(!(a))
    f = parser.parse("X ! a");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "X(!(a))");
}