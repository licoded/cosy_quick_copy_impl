#include "formula/simplify/release.hpp"
#include "formula/simplify/simplifier.hpp"
#include "formula/builder.hpp"
#include "formula/formula.hpp"

namespace Cosy {

Formula* ReleaseSimplifier::simplify(Formula* formula, FormulaBuilder& builder) {
    // Extract left and right operands and simplify them
    Formula* left = formula->left();
    Formula* right = formula->right();

    Formula* simplified_left = FormulaSimplifier::simplify(left, builder);
    Formula* simplified_right = FormulaSimplifier::simplify(right, builder);

    // Rule 1: True R a → a
    if (simplified_left->op() == Operator::True) {
        return simplified_right;
    }

    // Rule 2: a R False → False
    if (simplified_right->op() == Operator::False) {
        return builder.make_false();
    }

    // Rule 3: a R True → True
    if (simplified_right->op() == Operator::True) {
        return builder.make_true();
    }

    // Rule 4: a R (a & b) → a & b (right absorption)
    if (simplified_right->op() == Operator::And) {
        if (simplified_right->left() == simplified_left || simplified_right->right() == simplified_left) {
            return simplified_right;
        }
    }

    // Rule 5: (a | b) R a → a (left absorption)
    if (simplified_left->op() == Operator::Or) {
        if (simplified_left->left() == simplified_right || simplified_left->right() == simplified_right) {
            return simplified_right;
        }
    }

    // Rule 6: !a R a → False R a (not absorption)
    if (simplified_left->op() == Operator::Not && simplified_left->right() == simplified_right) {
        Formula* false_f = builder.make_false();
        return builder.make_binary(Operator::Release, false_f, simplified_right);
    }

    // Default: create Release with simplified operands
    // Hash consing will handle deduplication automatically
    return builder.make_binary(Operator::Release, simplified_left, simplified_right);
}

} // namespace Cosy