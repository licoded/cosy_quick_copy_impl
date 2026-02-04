#include "formula/formula.hpp"
#include "formula/builder.hpp"
#include <stdexcept>
#include <string>

namespace Cosy {

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
}

Formula::Formula(Operator op, Formula* left, Formula* right, unsigned int var_id)
    : op_(op), left_(left), right_(right), var_id_(var_id) {}

Formula::Formula(const std::string& str) {
    *this = *FormulaBuilder::parse(str);
}

Formula::~Formula() {
    delete left_;
    delete right_;
}

bool Formula::is_binary() const {
    return left_ != nullptr;
}


std::string Formula::toString() const {
    SymbolTable& symbol_table = get_global_symbol_table();

    // Atomic value (literal)
    if (left_ == nullptr && right_ == nullptr) {
        if (op_ == Operator::Literal) {
            return symbol_table.get_name(var_id_);
        }
        return symbol_table.get_name(static_cast<int>(op_));
    }

    // Error: invalid state
    if (left_ != nullptr && right_ == nullptr) {
        throw std::runtime_error("Invalid formula: binary operator without right operand");
    }

    const std::string& op_str = symbol_table.get_name(static_cast<int>(op_));

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

