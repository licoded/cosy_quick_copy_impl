#include "formula/formula.hpp"
#include "formula/synthesis_context.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("FormulaParser: Error handling", "[parser][errors]") {
    SynthesisContext ctx;

    // Incomplete expression: a &
    REQUIRE_THROWS(ctx.parse_formula("a &"));

    // Unmatched parenthesis: (a
    REQUIRE_THROWS(ctx.parse_formula("(a"));

    // Empty string
    REQUIRE_THROWS(ctx.parse_formula(""));

    // Extra tokens: a b
    REQUIRE_THROWS(ctx.parse_formula("a b"));

    // Unclosed parenthesis in complex expression
    REQUIRE_THROWS(ctx.parse_formula("a & (b | c"));
}