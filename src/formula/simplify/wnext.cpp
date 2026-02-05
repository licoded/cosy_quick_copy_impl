#include "formula/simplify/wnext.hpp"
#include "formula/simplify/simplifier.hpp"
#include "formula/builder.hpp"
#include "formula/formula.hpp"

namespace Cosy {

Formula* WNextSimplifier::simplify(Formula* formula, FormulaBuilder& builder) {
    // Extract operand and simplify it
    Formula* operand = formula->right();
    Formula* simplified_operand = FormulaSimplifier::simplify(operand, builder);

    // Rule 1: X True → True
    if (simplified_operand->op() == Operator::True) {
        return builder.make_true();
    }

    // Default: create WNext with simplified operand
    // Hash consing will handle deduplication automatically
    return builder.make_unary(Operator::WNext, simplified_operand);
}

} // namespace Cosy
