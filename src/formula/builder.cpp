#include "formula/builder.hpp"
#include "formula/formula.hpp"
#include "ltlparser/trans.h"
#include <stdexcept>

namespace Cosy {

namespace {
    // Forward declaration
    Formula* build_formula(const ltl_formula* ast);
}

SymbolTable& FormulaBuilder::get_symbol_table() {
    return get_global_symbol_table();
}

Formula* FormulaBuilder::parse(const std::string& input) {
    if (input.empty()) {
        throw std::invalid_argument("Input formula cannot be empty");
    }
    ltl_formula* ast = getAST(input.c_str());
    if (ast == nullptr) {
        throw std::runtime_error("Failed to parse formula");
    }
    Formula* formula = build_formula(ast);
    destroy_formula(ast);
    return formula;
}

Formula* FormulaBuilder::make_true() {
    return new Formula(Operator::True, nullptr, nullptr);
}

Formula* FormulaBuilder::make_false() {
    return new Formula(Operator::False, nullptr, nullptr);
}

Formula* FormulaBuilder::make_literal(const std::string& var_name) {
    unsigned int id = get_symbol_table().get_or_create_variable_id(var_name);
    return new Formula(Operator::Literal, nullptr, nullptr, id);
}

Formula* FormulaBuilder::make_unary(Operator op, Formula* sub_formula) {
    if (!is_unary_operator(op)) {
        throw std::invalid_argument("Invalid unary operator");
    }
    return new Formula(op, nullptr, sub_formula);
}

Formula* FormulaBuilder::make_binary(Operator op, Formula* left, Formula* right) {
    if (!is_binary_operator(op)) {
        throw std::invalid_argument("Invalid binary operator");
    }
    return new Formula(op, left, right);
}

namespace {

Formula* build_formula(const ltl_formula* ast) {
    if (ast == nullptr) {
        throw std::invalid_argument("AST node cannot be null");
    }

    Formula *left = ast->_left == nullptr ? nullptr : build_formula(ast->_left);
    Formula *right = ast->_right == nullptr ? nullptr : build_formula(ast->_right);

    switch (ast->_type) {
        case eTRUE:
            return FormulaBuilder::make_true();
        case eFALSE:
            return FormulaBuilder::make_false();
        case eLITERAL:
            return FormulaBuilder::make_literal(ast->_var);
        case eNOT:
            return FormulaBuilder::make_unary(Operator::Not, right);
        case eNEXT:
            return FormulaBuilder::make_unary(Operator::Next, right);
        case eWNEXT:
            return FormulaBuilder::make_unary(Operator::WNext, right);
        case eGLOBALLY:
            return FormulaBuilder::make_binary(Operator::Release, FormulaBuilder::make_false(), right);
        case eFUTURE:
            return FormulaBuilder::make_binary(Operator::Until, FormulaBuilder::make_true(), right);
        case eUNTIL:
            return FormulaBuilder::make_binary(Operator::Until, left, right);
        case eRELEASE:
            return FormulaBuilder::make_binary(Operator::Release, FormulaBuilder::make_false(), right);
        case eAND:
            return FormulaBuilder::make_binary(Operator::And, left, right);
        case eOR:
            return FormulaBuilder::make_binary(Operator::Or, left, right);
        case eIMPLIES: {
            Formula* not_left = FormulaBuilder::make_unary(Operator::Not, left);
            Formula* result = FormulaBuilder::make_binary(Operator::Or, not_left, right);
            return result;
        }
        case eEQUIV: {
            Formula* not_left = FormulaBuilder::make_unary(Operator::Not, left);
            Formula* not_right = FormulaBuilder::make_unary(Operator::Not, right);
            Formula* left_implies_right = FormulaBuilder::make_binary(Operator::Or, not_left, right);
            Formula* right_implies_left = FormulaBuilder::make_binary(Operator::Or, not_right, left);
            Formula* result = FormulaBuilder::make_binary(Operator::And, left_implies_right, right_implies_left);
            return result;
        }
        default:
            throw std::runtime_error("The formula cannot be recognized!");
    }
}

} // anonymous namespace

} // namespace Cosy
