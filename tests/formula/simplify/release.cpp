#include "formula/formula.hpp"
#include "formula/synthesis_context.hpp"
#include <catch2/catch_test_macros.hpp>

namespace Cosy {

TEST_CASE("Simplify Release: True R a → a", "[simplify][release]") {
    SynthesisContext ctx;
    Formula* a = ctx.parse_formula("a");
    Formula* true_release_a = ctx.parse_formula("true R a");

    // True R a should simplify to a
    Formula* simplified = true_release_a->simplify();
    REQUIRE(simplified == a);
}

TEST_CASE("Simplify Release: a R False → False", "[simplify][release]") {
    SynthesisContext ctx;
    Formula* false_f = ctx.parse_formula("false");
    Formula* a_release_false = ctx.parse_formula("a R false");

    // a R False should simplify to False
    Formula* simplified = a_release_false->simplify();
    REQUIRE(simplified == false_f);
}

TEST_CASE("Simplify Release: a R True → True", "[simplify][release]") {
    SynthesisContext ctx;
    Formula* true_f = ctx.parse_formula("true");
    Formula* a_release_true = ctx.parse_formula("a R true");

    // a R True should simplify to True
    Formula* simplified = a_release_true->simplify();
    REQUIRE(simplified == true_f);
}

TEST_CASE("Simplify Release: a R (a & b) → a & b", "[simplify][release]") {
    SynthesisContext ctx;
    Formula* a_and_b = ctx.parse_formula("a & b");
    Formula* nested = ctx.parse_formula("a R (a & b)");

    Formula* simplified = nested->simplify();
    REQUIRE(simplified == a_and_b);
}

TEST_CASE("Simplify Release: (a | b) R a → a", "[simplify][release]") {
    SynthesisContext ctx;
    Formula* a = ctx.parse_formula("a");
    Formula* nested = ctx.parse_formula("(a | b) R a");

    Formula* simplified = nested->simplify();
    REQUIRE(simplified == a);
}

TEST_CASE("Simplify Release: nested (true R a) R b → a R b", "[simplify][release]") {
    SynthesisContext ctx;
    Formula* a = ctx.parse_formula("a");
    Formula* b = ctx.parse_formula("b");
    Formula* nested = ctx.parse_formula("(true R a) R b");  // becomes (a R b)

    Formula* simplified = nested->simplify();
    Formula* expected = ctx.parse_formula("a R b");
    REQUIRE(simplified == expected);
}

TEST_CASE("Simplify Release: nested a R (b R false) → a R false", "[simplify][release]") {
    SynthesisContext ctx;
    Formula* nested = ctx.parse_formula("a R (b R false)");  // becomes (a R false), which simplifies to false

    Formula* simplified = nested->simplify();
    Formula* expected = ctx.parse_formula("false");
    REQUIRE(simplified == expected);
}

TEST_CASE("Simplify Release: nested (a | b) R (a | b) → unchanged", "[simplify][release]") {
    SynthesisContext ctx;
    Formula* original = ctx.parse_formula("(a | b) R (a | b)");
    Formula* simplified = original->simplify();

    // Should remain unchanged since none of the rules apply
    REQUIRE(simplified == original);
}

} // namespace Cosy
