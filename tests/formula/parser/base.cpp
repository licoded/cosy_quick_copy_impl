#include "formula/formula_parser.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace formula;

TEST_CASE("FormulaParser: Parse literals", "[parser]") {
    FormulaParser parser;

    auto f = parser.parse("true");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "true");
    REQUIRE(parser.error().empty());

    f = parser.parse("false");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "false");

    f = parser.parse("a");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "a");
    auto vars = Formula::collect_variables(f);
    REQUIRE(vars == std::unordered_set<std::string>{"a"});
}

TEST_CASE("FormulaParser: Parse unary operators", "[parser]") {
    FormulaParser parser;

    auto f = parser.parse("!a");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "!(a)");
 
    f = parser.parse("X a");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "X(a)");
 
    f = parser.parse("X (a & b)");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "X((a & b))");
 
    f = parser.parse("! X a");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "!(X(a))");
}

TEST_CASE("FormulaParser: Parse binary operators", "[parser]") {
    FormulaParser parser;

    auto f = parser.parse("a & b");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "(a & b)");

    f = parser.parse("a | b");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "(a | b)");
}

TEST_CASE("FormulaParser: Operator precedence", "[parser]") {
    FormulaParser parser;

    auto f = parser.parse("a & b | c");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "((a & b) | c)");

    f = parser.parse("a | b & c");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "(a | (b & c))");
}

TEST_CASE("FormulaParser: Parentheses", "[parser]") {
    FormulaParser parser;

    auto f = parser.parse("(a | b) & c");
    REQUIRE(f != nullptr);
    REQUIRE(f->to_string() == "((a | b) & c)");
}

TEST_CASE("FormulaParser: Error handling", "[parser]") {
    FormulaParser parser;

    auto f = parser.parse("");
    REQUIRE(f == nullptr);
    REQUIRE(!parser.error().empty());

    f = parser.parse("a &");
    REQUIRE(f == nullptr);

    f = parser.parse("(a");
    REQUIRE(f == nullptr);
}