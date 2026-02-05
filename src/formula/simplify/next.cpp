#include "formula/simplify/next.hpp"
#include "formula/simplify/simplifier.hpp"
#include "formula/builder.hpp"
#include "formula/formula.hpp"

namespace Cosy {

Formula* NextSimplifier::simplify(Formula* operand, FormulaBuilder& builder) {
    // First simplify the operand
    Formula* simplified_operand = FormulaSimplifier::simplify(operand, builder);

    // Rule 1: X[!] False → False
    if (simplified_operand->op() == Operator::False) {
        return builder.make_false();
    }

    // Default: create Next with simplified operand
    // Hash consing will handle deduplication automatically
    return builder.make_unary(Operator::Next, simplified_operand);
}

} // namespace Cosy
