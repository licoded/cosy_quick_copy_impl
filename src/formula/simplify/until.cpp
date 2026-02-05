#include "formula/simplify/until.hpp"
#include "formula/simplify/simplifier.hpp"
#include "formula/builder.hpp"
#include "formula/formula.hpp"

namespace Cosy {

Formula* UntilSimplifier::simplify(Formula* formula, FormulaBuilder& builder) {
    // Extract left and right operands and simplify them
    Formula* left = formula->left();
    Formula* right = formula->right();

    Formula* simplified_left = FormulaSimplifier::simplify(left, builder);
    Formula* simplified_right = FormulaSimplifier::simplify(right, builder);

    // Rule 1: False U a → a
    if (simplified_left->op() == Operator::False) {
        return simplified_right;
    }

    // Rule 2: a U False → False
    if (simplified_right->op() == Operator::False) {
        return builder.make_false();
    }

    // Rule 3: True U a → True
    if (simplified_left->op() == Operator::True) {
        return builder.make_true();
    }

    // Rule 4: a U True → True
    if (simplified_right->op() == Operator::True) {
        return builder.make_true();
    }

    // Rule 5: a U a → a
    if (simplified_left == simplified_right) {
        return simplified_left;
    }

    // Default: create Until with simplified operands
    // Hash consing will handle deduplication automatically
    return builder.make_binary(Operator::Until, simplified_left, simplified_right);
}

} // namespace Cosy