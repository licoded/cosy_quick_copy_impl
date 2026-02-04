#include "formula/formula_parser.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("FormulaParser: Error handling", "[parser][errors]") {
    // Incomplete expression: a &
    REQUIRE_THROWS(new Formula("a &"));

    // Unmatched parenthesis: (a
    REQUIRE_THROWS(new Formula("(a"));

    // Empty string
    REQUIRE_THROWS(new Formula(""));

    // Extra tokens: a b
    REQUIRE_THROWS(new Formula("a b"));

    // Unclosed parenthesis in complex expression
    REQUIRE_THROWS(new Formula("a & (b | c"));
}