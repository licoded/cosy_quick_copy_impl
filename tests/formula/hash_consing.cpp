#include "formula/formula.hpp"
#include "formula/synthesis_context.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

TEST_CASE("HashConsing: True singleton", "[hash_consing]") {
    SynthesisContext ctx;
    Formula* t1 = ctx.parse_formula("true");
    Formula* t2 = ctx.parse_formula("true");

    REQUIRE(t1 == t2);
    REQUIRE(t1->toString() == "true");
}

TEST_CASE("HashConsing: False singleton", "[hash_consing]") {
    SynthesisContext ctx;
    Formula* f1 = ctx.parse_formula("false");
    Formula* f2 = ctx.parse_formula("false");

    REQUIRE(f1 == f2);
    REQUIRE(f1->toString() == "false");
}

TEST_CASE("HashConsing: Literal deduplication", "[hash_consing]") {
    SynthesisContext ctx;
    Formula* a1 = ctx.parse_formula("a");
    Formula* a2 = ctx.parse_formula("a");

    REQUIRE(a1 == a2);
    REQUIRE(a1->toString() == "a");
}

TEST_CASE("HashConsing: Structural deduplication", "[hash_consing]") {
    SynthesisContext ctx;
    Formula* ab1 = ctx.parse_formula("a & b");
    Formula* ab2 = ctx.parse_formula("a & b");

    REQUIRE(ab1 == ab2);
}

TEST_CASE("HashConsing: AND commutativity", "[hash_consing][canonicalization]") {
    SynthesisContext ctx;
    Formula* ab = ctx.parse_formula("a & b");
    Formula* ba = ctx.parse_formula("b & a");

    // With commutativity optimization, they are canonicalized at creation time
    REQUIRE(ab == ba);

    // Hash values should also be identical
    REQUIRE(ab->hash() == ba->hash());
}

TEST_CASE("HashConsing: OR commutativity", "[hash_consing][canonicalization]") {
    SynthesisContext ctx;
    Formula* ab = ctx.parse_formula("a | b");
    Formula* ba = ctx.parse_formula("b | a");

    // With commutativity optimization, they are canonicalized at creation time
    REQUIRE(ab == ba);

    // Hash values should also be identical
    REQUIRE(ab->hash() == ba->hash());
}
