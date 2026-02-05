#include "formula/formula.hpp"
#include "formula/synthesis_context.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("FormulaParser: Manual test - X((a | b) U c)", "[parser][manual]") {
    SynthesisContext ctx;
    std::string input = "X((a | b) U c)";
    Formula* f = ctx.parse_formula(input);
    std::string output = f->toString();

    // Check if toString preserves the original format
    REQUIRE(input == output);
}
