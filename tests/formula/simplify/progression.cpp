#include "formula/formula.hpp"
#include "formula/synthesis_context.hpp"
#include <catch2/catch_test_macros.hpp>
#include <unordered_set>

using namespace Cosy;

TEST_CASE("Simplify: progression result of G(a->Xb) with no inputs", "[simplify][progression]") {
    SynthesisContext ctx;
    Formula* init = ctx.parse_formula("G(a -> X b) & F(true)");
    int id_a = ctx.symbols().get_or_create_variable_id("a");
    int id_b = ctx.symbols().get_or_create_variable_id("b");

    // va=0, vb=0: lits = {-id_a, -id_b}，产生 true & (false | G(!a | Xb))
    std::unordered_set<int> lits = {-id_a, -id_b};
    Formula* progressed = init->nnf()->xnf()->progression(lits);
    Formula* expected   = ctx.parse_formula("G(a -> X b)");

    REQUIRE(progressed == expected);
}
