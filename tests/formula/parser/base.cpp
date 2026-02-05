#include "formula/formula.hpp"
#include "formula/synthesis_context.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("FormulaParser: Parse literals", "[parser]") {
    SynthesisContext ctx;
    Formula* f1 = ctx.parse_formula("true");
    REQUIRE(f1->toString() == "true");

    Formula* f2 = ctx.parse_formula("false");
    REQUIRE(f2->toString() == "false");

    Formula* f3 = ctx.parse_formula("a");
    REQUIRE(f3->toString() == "a");
}

TEST_CASE("FormulaParser: Parse unary operators", "[parser]") {
    SynthesisContext ctx;
    Formula* f1 = ctx.parse_formula("!a");
    REQUIRE(f1->toString() == "!a");

    Formula* f2 = ctx.parse_formula("X a");
    REQUIRE(f2->toString() == "Xa");

    Formula* f3 = ctx.parse_formula("X (a & b)");
    REQUIRE(f3->toString() == "X(a & b)");

    Formula* f4 = ctx.parse_formula("! X a");
    REQUIRE(f4->toString() == "!Xa");
}

TEST_CASE("FormulaParser: Parse binary operators", "[parser]") {
    SynthesisContext ctx;
    Formula* f1 = ctx.parse_formula("a & b");
    REQUIRE(f1->toString() == "a & b");

    Formula* f2 = ctx.parse_formula("a | b");
    REQUIRE(f2->toString() == "a | b");
}

TEST_CASE("FormulaParser: Operator precedence", "[parser]") {
    SynthesisContext ctx;
    Formula* f1 = ctx.parse_formula("a & b | c");
    REQUIRE(f1->toString() == "(a & b) | c");

    Formula* f2 = ctx.parse_formula("a | b & c");
    REQUIRE(f2->toString() == "a | (b & c)");
}

TEST_CASE("FormulaParser: Parentheses", "[parser]") {
    SynthesisContext ctx;
    Formula* f1 = ctx.parse_formula("(a | b) & c");
    REQUIRE(f1->toString() == "(a | b) & c");
}

TEST_CASE("FormulaParser: Error handling", "[parser]") {
    SynthesisContext ctx;
    REQUIRE_THROWS(ctx.parse_formula(""));
    REQUIRE_THROWS(ctx.parse_formula("a &"));
    REQUIRE_THROWS(ctx.parse_formula("(a"));
}
