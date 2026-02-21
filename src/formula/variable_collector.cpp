#include "formula/variable_collector.hpp"
#include "formula/operator.hpp"

namespace Cosy {

void VariableCollector::visit(Formula* formula) {
    // 根据操作符类型分发到具体方法
    switch (formula->op()) {
        case Operator::True:
            visit_true(formula);
            break;
        case Operator::False:
            visit_false(formula);
            break;
        case Operator::Literal:
            visit_literal(formula);
            break;
        case Operator::Not:
            visit_not(formula);
            break;
        case Operator::And:
            visit_and(formula);
            break;
        case Operator::Or:
            visit_or(formula);
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
        default:
            break;
    }
}

void VariableCollector::visit_binary(Formula* formula) {
    if (formula->left()) {
        formula->left()->accept(*this);
    }
    if (formula->right()) {
        formula->right()->accept(*this);
    }
}

void VariableCollector::visit_unary(Formula* formula) {
    if (formula->right()) {
        formula->right()->accept(*this);
    }
}

void VariableCollector::visit_literal(Formula* formula) {
    var_ids_.insert(formula->var_id());
}

void VariableCollector::visit_constant(Formula* /*formula*/) {
    // 常量没有变量，不需要做任何事情
}

void VariableCollector::visit_and(Formula* formula) {
    visit_binary(formula);
}

void VariableCollector::visit_or(Formula* formula) {
    visit_binary(formula);
}

void VariableCollector::visit_not(Formula* formula) {
    visit_unary(formula);
}

void VariableCollector::visit_next(Formula* formula) {
    visit_unary(formula);
}

void VariableCollector::visit_wnext(Formula* formula) {
    visit_unary(formula);
}

void VariableCollector::visit_until(Formula* formula) {
    visit_binary(formula);
}

void VariableCollector::visit_release(Formula* formula) {
    visit_binary(formula);
}

void VariableCollector::visit_true(Formula* /*formula*/) {
    visit_constant(nullptr);
}

void VariableCollector::visit_false(Formula* /*formula*/) {
    visit_constant(nullptr);
}

std::unordered_set<unsigned int> collect_variables(Formula* formula) {
    VariableCollector collector;
    if (formula) {
        formula->accept(collector);
    }
    return collector.get_var_ids();
}

} // namespace Cosy
