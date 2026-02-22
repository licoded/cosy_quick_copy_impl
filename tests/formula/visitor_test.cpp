#include "formula/formula.hpp"
#include "formula/visitor.hpp"
#include "formula/synthesis_context.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Cosy;

class CountVisitor : public Visitor {
public:
    int count = 0;

    void visit(Formula* formula) override {
        count++;

        // 递归访问子节点
        if (formula->left()) {
            formula->left()->accept(*this);
        }
        if (formula->right()) {
            formula->right()->accept(*this);
        }
    }

    void visit_binary(Formula* formula) override {
        visit(formula);
    }

    void visit_unary(Formula* formula) override {
        visit(formula);
    }

    void visit_literal(Formula* formula) override {
        visit(formula);
    }

    void visit_constant(Formula* formula) override {
        visit(formula);
    }

    void visit_and(Formula* formula) override {
        visit(formula);
    }

    void visit_or(Formula* formula) override {
        visit(formula);
    }

    void visit_not(Formula* formula) override {
        visit(formula);
    }

    void visit_next(Formula* formula) override {
        visit(formula);
    }

    void visit_wnext(Formula* formula) override {
        visit(formula);
    }

    void visit_until(Formula* formula) override {
        visit(formula);
    }

    void visit_release(Formula* formula) override {
        visit(formula);
    }

    void visit_true(Formula* formula) override {
        visit(formula);
    }

    void visit_false(Formula* formula) override {
        visit(formula);
    }
};

TEST_CASE("Visitor Pattern Test: Formula traversal", "[visitor]") {
    SynthesisContext ctx;
    Formula* formula = ctx.parse_formula("a & b");

    CountVisitor visitor;
    formula->accept(visitor);

    // Formula "a & b" should have 3 nodes: a, b, and (a & b)
    REQUIRE(visitor.count == 3);
}

TEST_CASE("Visitor Pattern Test: Nested formula traversal", "[visitor]") {
    SynthesisContext ctx;
    Formula* formula = ctx.parse_formula("(a & b) | c");

    CountVisitor visitor;
    formula->accept(visitor);

    // Formula "(a & b) | c" should have 5 nodes: a, b, (a & b), c, ((a & b) | c)
    REQUIRE(visitor.count == 5);
}

TEST_CASE("Visitor Pattern Test: Formula simplification using traditional approach", "[visitor][simplify]") {
    SynthesisContext ctx;
    Formula* formula = ctx.parse_formula("X[!] (a & false)");  // Should simplify to X[!] false and then to false with existing simplifier

    Formula* simplified = formula->simplify();

    REQUIRE(simplified->op() == Operator::False);
}