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

    // Rule 6: a U (a | b) → a | b (right absorption)
    if (simplified_right->op() == Operator::Or) {
        if (simplified_right->left() == simplified_left || simplified_right->right() == simplified_left) {
            return simplified_right;
        }
    }

    // Rule 7: a U (a U b) → a U b
    if (simplified_right->op() == Operator::Until && simplified_right->left() == simplified_left) {
        return builder.make_binary(Operator::Until, simplified_left, simplified_right->right());
    }

    // Rule 8: a U (b U a) → b U a
    if (simplified_right->op() == Operator::Until && simplified_right->right() == simplified_left) {
        return simplified_right;
    }

    // Rule 9: (X[!] a) U a → a | (X[!] a)
    if (simplified_left->op() == Operator::Next && simplified_left->right() == simplified_right) {
        return builder.make_binary(Operator::Or, simplified_right, simplified_left);
    }

    // Rule 10: (X[!] a) U (X[!] b) → X[!](a U b)
    if (simplified_left->op() == Operator::Next && simplified_right->op() == Operator::Next) {
        Formula* inner_until = builder.make_binary(Operator::Until, simplified_left->right(), simplified_right->right());
        return builder.make_unary(Operator::Next, inner_until);
    }

    // Default: create Until with simplified operands
    // Hash consing will handle deduplication automatically
    return builder.make_binary(Operator::Until, simplified_left, simplified_right);
}

} // namespace Cosy