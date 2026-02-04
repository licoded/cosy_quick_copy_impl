#include "formula/formula_parser.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("FormulaParser: Error handling", "[parser][errors]") {
    // Incomplete expression: a &
    Formula* f = new Formula("a &");
    REQUIRE(f == nullptr);
    

    // Unmatched parenthesis: (a
    f = new Formula("(a");
    REQUIRE(f == nullptr);
    

    // Empty string
    f = new Formula("");
    REQUIRE(f == nullptr);
    

    // Invalid token: a @
    f = new Formula("a @");
    REQUIRE(f == nullptr);
    

    // Extra tokens: a b
    f = new Formula("a b");
    REQUIRE(f == nullptr);
    

    // Unclosed parenthesis in complex expression
    f = new Formula("a & (b | c");
    REQUIRE(f == nullptr);
    
}