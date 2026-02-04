#include "formula/formula_parser.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("FormulaParser: Parse literals", "[parser]") {
    Formula f1("true");
    REQUIRE(f1.toString() == "true");

    Formula f2("false");
    REQUIRE(f2.toString() == "false");

    Formula f3("a");
    REQUIRE(f3.toString() == "a");
}

TEST_CASE("FormulaParser: Parse unary operators", "[parser]") {
    Formula f1("!a");
    REQUIRE(f1.toString() == "!(a)");

    Formula f2("X a");
    REQUIRE(f2.toString() == "X(a)");

    Formula f3("X (a & b)");
    REQUIRE(f3.toString() == "X(a & b)");

    Formula f4("! X a");
    REQUIRE(f4.toString() == "!(X(a))");
}

TEST_CASE("FormulaParser: Parse binary operators", "[parser]") {
    Formula f1("a & b");
    REQUIRE(f1.toString() == "(a & b)");

    Formula f2("a | b");
    REQUIRE(f2.toString() == "(a | b)");
}

TEST_CASE("FormulaParser: Operator precedence", "[parser]") {
    Formula f1("a & b | c");
    REQUIRE(f1.toString() == "((a & b) | c)");

    Formula f2("a | b & c");
    REQUIRE(f2.toString() == "(a | (b & c))");
}

TEST_CASE("FormulaParser: Parentheses", "[parser]") {
    Formula f1("(a | b) & c");
    REQUIRE(f1.toString() == "((a | b) & c)");
}

TEST_CASE("FormulaParser: Error handling", "[parser]") {
    REQUIRE_THROWS(Formula(""));
    REQUIRE_THROWS(Formula("a &"));
    REQUIRE_THROWS(Formula("(a"));
}
