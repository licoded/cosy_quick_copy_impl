#include "formula/builder.hpp"
#include "formula/formula.hpp"
#include "formula/synthesis_context.hpp"
#include "ltlparser/trans.h"
#include <stdexcept>

namespace Cosy {

namespace {
    // Forward declaration
    Formula* build_formula(FormulaBuilder& builder, const ltl_formula* ast);
}

FormulaBuilder::FormulaBuilder(SynthesisContext& context)
    : context_(context) {}

Formula* FormulaBuilder::parse(const std::string& input) {
    if (input.empty()) {
        throw std::invalid_argument("Input formula cannot be empty");
    }
    ltl_formula* ast = getAST(input.c_str());
    if (ast == nullptr) {
        throw std::runtime_error("Failed to parse formula");
    }
    Formula* formula = build_formula(*this, ast);
    destroy_formula(ast);
    return formula;
}

Formula* FormulaBuilder::make_true() {
    return new Formula(Operator::True, nullptr, nullptr, 0, &context_);
}

Formula* FormulaBuilder::make_false() {
    return new Formula(Operator::False, nullptr, nullptr, 0, &context_);
}

Formula* FormulaBuilder::make_literal(const std::string& var_name) {
    unsigned int id = context_.symbols().get_or_create_variable_id(var_name);
    return new Formula(Operator::Literal, nullptr, nullptr, id, &context_);
}

Formula* FormulaBuilder::make_unary(Operator op, Formula* sub_formula) {
    if (!is_unary_operator(op)) {
        throw std::invalid_argument("Invalid unary operator");
    }
    return new Formula(op, nullptr, sub_formula, 0, &context_);
}

Formula* FormulaBuilder::make_binary(Operator op, Formula* left, Formula* right) {
    if (!is_binary_operator(op)) {
        throw std::invalid_argument("Invalid binary operator");
    }
    return new Formula(op, left, right, 0, &context_);
}

namespace {

Formula* build_formula(FormulaBuilder& builder, const ltl_formula* ast) {
    if (ast == nullptr) {
        throw std::invalid_argument("AST node cannot be null");
    }

    Formula *left = ast->_left == nullptr ? nullptr : build_formula(builder, ast->_left);
    Formula *right = ast->_right == nullptr ? nullptr : build_formula(builder, ast->_right);

    switch (ast->_type) {
        case eTRUE:
            return builder.make_true();
        case eFALSE:
            return builder.make_false();
        case eLITERAL:
            return builder.make_literal(ast->_var);
        case eNOT:
            return builder.make_unary(Operator::Not, right);
        case eNEXT:
            return builder.make_unary(Operator::Next, right);
        case eWNEXT:
            return builder.make_unary(Operator::WNext, right);
        case eGLOBALLY:
            return builder.make_binary(Operator::Release, builder.make_false(), right);
        case eFUTURE:
            return builder.make_binary(Operator::Until, builder.make_true(), right);
        case eUNTIL:
            return builder.make_binary(Operator::Until, left, right);
        case eRELEASE:
            return builder.make_binary(Operator::Release, builder.make_false(), right);
        case eAND:
            return builder.make_binary(Operator::And, left, right);
        case eOR:
            return builder.make_binary(Operator::Or, left, right);
        case eIMPLIES: {
            Formula* not_left = builder.make_unary(Operator::Not, left);
            Formula* result = builder.make_binary(Operator::Or, not_left, right);
            return result;
        }
        case eEQUIV: {
            Formula* not_left = builder.make_unary(Operator::Not, left);
            Formula* not_right = builder.make_unary(Operator::Not, right);
            Formula* left_implies_right = builder.make_binary(Operator::Or, not_left, right);
            Formula* right_implies_left = builder.make_binary(Operator::Or, not_right, left);
            Formula* result = builder.make_binary(Operator::And, left_implies_right, right_implies_left);
            return result;
        }
        default:
            throw std::runtime_error("The formula cannot be recognized!");
    }
}

} // anonymous namespace

} // namespace Cosy
