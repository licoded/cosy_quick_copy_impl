#include "formula/stringifier.hpp"
#include "formula/formula.hpp"
#include "formula/operator.hpp"
#include <stdexcept>

namespace Cosy {

std::string FormulaStringifier::parenthesize_if_binary(const Formula* f) {
    std::string s = f->toString();
    if (f->is_binary()) {
        s = "(" + s + ")";
    }
    return s;
}

std::string FormulaStringifier::format_binary(const Formula* left, const std::string& op,
                                               const Formula* right) {
    std::string left_str = parenthesize_if_binary(left);
    std::string right_str = parenthesize_if_binary(right);
    return left_str + " " + op + " " + right_str;
}

std::string FormulaStringifier::to_string(const Formula* formula) {
    SymbolTable& symbol_table = get_global_symbol_table();

    // Atomic value (literal)
    if (formula->left_ == nullptr && formula->right_ == nullptr) {
        if (formula->op_ == Operator::Literal) {
            return symbol_table.get_var_name(formula->var_id_);
        }
        return symbol_table.get_op_str(formula->op_);
    }

    // Error: invalid state
    if (formula->left_ != nullptr && formula->right_ == nullptr) {
        throw std::runtime_error("Invalid formula: binary operator without right operand");
    }

    const std::string& op_str = symbol_table.get_op_str(formula->op_);

    // Unary prefix operators: Not, Next, WNext
    if (!formula->is_binary()) {
        return op_str + parenthesize_if_binary(formula->right_);
    }

    // F(uture), G(lobal)
    if (formula->left_->op_ == Operator::True && formula->op_ == Operator::Until)
        return "F" + parenthesize_if_binary(formula->right_);
    if (formula->left_->op_ == Operator::False && formula->op_ == Operator::Release)
        return "G" + parenthesize_if_binary(formula->right_);

    return format_binary(formula->left_, op_str, formula->right_);
}

} // namespace Cosy
