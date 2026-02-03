#include "formula/formula_parser.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace formula;

TEST_CASE("FormulaParser: Error handling", "[parser][errors]") {
    FormulaParser parser;

    // Incomplete expression: a &
    auto f = parser.parse("a &");
    REQUIRE(f == nullptr);
    REQUIRE(!parser.error().empty());

    // Unmatched parenthesis: (a
    f = parser.parse("(a");
    REQUIRE(f == nullptr);
    REQUIRE(!parser.error().empty());

    // Empty string
    f = parser.parse("");
    REQUIRE(f == nullptr);
    REQUIRE(!parser.error().empty());

    // Invalid token: a @
    f = parser.parse("a @");
    REQUIRE(f == nullptr);
    REQUIRE(!parser.error().empty());

    // Extra tokens: a b
    f = parser.parse("a b");
    REQUIRE(f == nullptr);
    REQUIRE(!parser.error().empty());

    // Unclosed parenthesis in complex expression
    f = parser.parse("a & (b | c");
    REQUIRE(f == nullptr);
    REQUIRE(!parser.error().empty());
}