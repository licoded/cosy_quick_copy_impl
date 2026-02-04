#include "formula/formula.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("FormulaParser: Binary operator precedence", "[parser][precedence]") {
    // & has higher precedence than |: a & b | c should be (a & b) | c
    Formula* f = new Formula("a & b | c");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "(a & b) | c");


    // | has lower precedence: a | b & c should be a | (b & c)
    f = new Formula("a | b & c");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "a | (b & c)");

    // Test associativity: a & b & c should be (a & b) & c (left-associative)
    f = new Formula("a & b & c");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "(a & b) & c");

    // a | b | c should be (a | b) | c
    f = new Formula("a | b | c");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "(a | b) | c");
}

TEST_CASE("FormulaParser: Parentheses override precedence", "[parser][precedence]") {
    // (a | b) & c should be (a | b) & c
    auto f = new Formula("(a | b) & c");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "(a | b) & c");

    // a & (b | c) should be a & (b | c)
    f = new Formula("a & (b | c)");
    REQUIRE(f != nullptr);
    REQUIRE(f->toString() == "a & (b | c)");
}