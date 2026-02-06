#include "formula/simplification_visitor.hpp"
#include "formula/simplify/simplifier.hpp"
#include "formula/simplify/and.hpp"
#include "formula/simplify/or.hpp"
#include "formula/simplify/next.hpp"
#include "formula/simplify/wnext.hpp"
#include "formula/simplify/until.hpp"
#include "formula/simplify/release.hpp"

namespace Cosy {

SimplificationVisitor::SimplificationVisitor(FormulaBuilder& builder)
    : builder_(builder) {}

void SimplificationVisitor::visit(Formula* formula) {
    switch (formula->op()) {
        case Operator::And:
            visit_and(formula);
            break;
        case Operator::Or:
            visit_or(formula);
            break;
        case Operator::Not:
            visit_not(formula);
            break;
        case Operator::Next:
            visit_next(formula);
            break;
        case Operator::WNext:
            visit_wnext(formula);
            break;
        case Operator::Until:
            visit_until(formula);
            break;
        case Operator::Release:
            visit_release(formula);
            break;
        case Operator::Literal:
            visit_literal(formula);
            break;
        case Operator::True:
            visit_true(formula);
            break;
        case Operator::False:
            visit_false(formula);
            break;
        default:
            // 对于未处理的操作符，直接返回原公式
            result_ = formula;
            break;
    }
}

void SimplificationVisitor::visit_binary(Formula* formula) {
    // 使用通用访问方法，它会根据操作符类型调用特定方法
    visit(formula);
}

void SimplificationVisitor::visit_unary(Formula* formula) {
    // 使用通用访问方法，它会根据操作符类型调用特定方法
    visit(formula);
}

void SimplificationVisitor::visit_literal(Formula* formula) {
    // 字面量通常不需要简化
    result_ = formula;
}

void SimplificationVisitor::visit_constant(Formula* formula) {
    // 常量不需要简化，但会根据具体类型处理
    visit(formula);
}

void SimplificationVisitor::visit_and(Formula* formula) {
    result_ = AndSimplifier::simplify(formula, builder_);
}

void SimplificationVisitor::visit_or(Formula* formula) {
    result_ = OrSimplifier::simplify(formula, builder_);
}

void SimplificationVisitor::visit_not(Formula* formula) {
    // Not 通常不需要特殊简化规则，递归简化子表达式
    Formula* simplified_sub = FormulaSimplifier::simplify(formula->right(), builder_);
    result_ = builder_.make_unary(Operator::Not, simplified_sub);
}

void SimplificationVisitor::visit_next(Formula* formula) {
    result_ = NextSimplifier::simplify(formula, builder_);
}

void SimplificationVisitor::visit_wnext(Formula* formula) {
    result_ = WNextSimplifier::simplify(formula, builder_);
}

void SimplificationVisitor::visit_until(Formula* formula) {
    result_ = UntilSimplifier::simplify(formula, builder_);
}

void SimplificationVisitor::visit_release(Formula* formula) {
    result_ = ReleaseSimplifier::simplify(formula, builder_);
}

void SimplificationVisitor::visit_true(Formula* formula) {
    result_ = builder_.make_true();
}

void SimplificationVisitor::visit_false(Formula* formula) {
    result_ = builder_.make_false();
}

} // namespace Cosy