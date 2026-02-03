#include "formula/formula_parser.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace formula;

TEST_CASE("FormulaParser: Binary operator precedence", "[parser][precedence]") {
    FormulaParser parser;

    // & has higher precedence than |: a & b | c should be (a & b) | c
    auto f = parser.parse("a & b | c");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "((a & b) | c)");
    REQUIRE(parser.error().empty());

    // | has lower precedence: a | b & c should be a | (b & c)
    f = parser.parse("a | b & c");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "(a | (b & c))");

    // Test associativity: a & b & c should be a & (b & c)
    f = parser.parse("a & b & c");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "(a & (b & c))");

    // a | b | c should be (a | b) | c
    f = parser.parse("a | b | c");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "((a | b) | c)");
}

TEST_CASE("FormulaParser: Parentheses override precedence", "[parser][precedence]") {
    FormulaParser parser;

    // (a | b) & c should be (a | b) & c
    auto f = parser.parse("(a | b) & c");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "((a | b) & c)");

    // a & (b | c) should be a & (b | c)
    f = parser.parse("a & (b | c)");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "(a & (b | c))");
}