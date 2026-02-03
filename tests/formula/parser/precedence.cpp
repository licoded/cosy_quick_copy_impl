#include "formula/formula_parser.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("FormulaParser: Binary operator precedence", "[parser][precedence]") {
    Formula parser;

    // & has higher precedence than |: a & b | c should be (a & b) | c
    auto f = parser.parse("a & b | c");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "((a & b) | c)");
    REQUIRE(parser.error().empty());

    // | has lower precedence: a | b & c should be a | (b & c)
    f = parser.parse("a | b & c");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "(a | (b & c))");

    // Test associativity: a & b & c should be a & (b & c)
    f = parser.parse("a & b & c");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "(a & (b & c))");

    // a | b | c should be (a | b) | c
    f = parser.parse("a | b | c");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "((a | b) | c)");
}

TEST_CASE("FormulaParser: Parentheses override precedence", "[parser][precedence]") {
    Formula parser;

    // (a | b) & c should be (a | b) & c
    auto f = parser.parse("(a | b) & c");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "((a | b) & c)");

    // a & (b | c) should be a & (b | c)
    f = parser.parse("a & (b | c)");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "(a & (b | c))");
}