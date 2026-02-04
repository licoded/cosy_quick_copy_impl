#include "formula/formula.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("FormulaParser: Manual test - X((a | b) U c)", "[parser][manual]") {
    std::string input = "X((a | b) U c)";
    Formula f(input.c_str());
    std::string output = f.toString();

    // Check if toString preserves the original format
    REQUIRE(input == output);
}
