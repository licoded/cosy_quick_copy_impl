#include "formula/formula.hpp"
#include "ltlparser/trans.h"
#include <stdexcept>
#include <cassert>
#include <string>

namespace Cosy {

SymbolTable Formula::symbol_table_;

namespace {
    // Forward declaration
    Formula* build_formula(const ltl_formula* ast);
}

Formula::Formula(Operator op, Formula* left, Formula* right, unsigned int var_id)
    : op_(op), left_(left), right_(right), var_id_(var_id) {}

Formula::Formula(const std::string& str) {
    *this = *Formula::parse(str);
}

Formula* Formula::parse(const std::string& input) {
    symbol_table_.initialize_operators();
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

Formula::~Formula() {
    delete left_;
    delete right_;
}

bool Formula::is_binary() const {
    return left_ != nullptr;
}

Formula *Formula::make_true() {
    return new Formula(Operator::True, nullptr, nullptr);
}

Formula *Formula::make_false() {
    return new Formula(Operator::False, nullptr, nullptr);
}

Formula* Formula::make_literal(const std::string& var_name) {
    unsigned int id = symbol_table_.get_or_create_variable_id(var_name);
    return new Formula(Operator::Literal, nullptr, nullptr, id);
}

Formula* Formula::make_unary(Operator op, Formula* sub_formula) {
    if (op != Operator::Not && op != Operator::Next && op != Operator::WNext) {
        throw std::invalid_argument("Invalid unary operator");
    }
    return new Formula(op, nullptr, sub_formula);
}

Formula* Formula::make_binary(Operator op, Formula* left, Formula* right) {
    if (op != Operator::And && op != Operator::Or && op != Operator::Until && op != Operator::Release) {
        throw std::invalid_argument("Invalid binary operator");
    }
    return new Formula(op, left, right);
}

namespace {

// Helper: parenthesize binary expressions (literals and unary need no parens)
std::string parenthesize_if_binary(const Formula* f) {
    std::string s = f->toString();
    if (f->is_binary()) {
        s = "(" + s + ")";
    }
    return s;
}

// Helper: format binary operator expression with proper parentheses
std::string format_binary(const Formula* left, const std::string& op, const Formula* right) {
    std::string left_str = parenthesize_if_binary(left);
    std::string right_str = parenthesize_if_binary(right);
    return left_str + " " + op + " " + right_str;
}

Formula* build_formula(const ltl_formula* ast) {
    if (ast == nullptr) {
        throw std::invalid_argument("AST node cannot be null");
    }

    Formula *left = ast->_left == nullptr ? nullptr : build_formula(ast->_left);
    Formula *right = ast->_right == nullptr ? nullptr : build_formula(ast->_right);

    switch (ast->_type) {
        case eTRUE:
            return Formula::make_true();
        case eFALSE:
            return Formula::make_false();
        case eLITERAL:
            return Formula::make_literal(ast->_var);
        case eNOT:
            return Formula::make_unary(Operator::Not, right);
        case eNEXT:
            return Formula::make_unary(Operator::Next, right);
        case eWNEXT:
            return Formula::make_unary(Operator::WNext, right);
        case eGLOBALLY:
            return Formula::make_binary(Operator::Release, Formula::make_false(), right);
        case eFUTURE:
            return Formula::make_binary(Operator::Until, Formula::make_true(), right);
        case eUNTIL:
            return Formula::make_binary(Operator::Until, left, right);
        case eRELEASE:
            return Formula::make_binary(Operator::Release, Formula::make_false(), right);
        case eAND:
            return Formula::make_binary(Operator::And, left, right);
        case eOR:
            return Formula::make_binary(Operator::Or, left, right);
        case eIMPLIES: {
            Formula* not_left = Formula::make_unary(Operator::Not, left);
            Formula* result = Formula::make_binary(Operator::Or, not_left, right);
            return result;
        }
        case eEQUIV: {
            Formula* not_left = Formula::make_unary(Operator::Not, left);
            Formula* not_right = Formula::make_unary(Operator::Not, right);
            Formula* left_implies_right = Formula::make_binary(Operator::Or, not_left, right);
            Formula* right_implies_left = Formula::make_binary(Operator::Or, not_right, left);
            Formula* result = Formula::make_binary(Operator::And, left_implies_right, right_implies_left);
            return result;
        }
        default:
            throw std::runtime_error("The formula cannot be recognized!");
    }
}

} // anonymous namespace


std::string Formula::toString() const {
    // Atomic value (literal)
    if (left_ == nullptr && right_ == nullptr) {
        if (op_ == Operator::Literal) {
            return symbol_table_.get_name(var_id_);
        }
        return symbol_table_.get_name(static_cast<int>(op_));
    }

    // Error: invalid state
    if (left_ != nullptr && right_ == nullptr) {
        throw std::runtime_error("Invalid formula: binary operator without right operand");
    }

    const std::string& op_str = symbol_table_.get_name(static_cast<int>(op_));

    // Unary prefix operators: Not, Next, WNext
    if (!is_binary()) {
        return op_str + parenthesize_if_binary(right_);
    }

    // F(uture), G(lobal)
    if (left_->op_ == Operator::True && op_ == Operator::Until)
        return "F" + parenthesize_if_binary(right_);
    if (left_->op_ == Operator::False && op_ == Operator::Release)
        return "G" + parenthesize_if_binary(right_);

    return format_binary(left_, op_str, right_);
}

} // namespace Cosy

