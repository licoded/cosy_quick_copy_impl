#include "formula/simplify/simplifier.hpp"
#include "formula/simplify/and.hpp"
#include "formula/simplify/or.hpp"
#include "formula/simplify/next.hpp"
#include "formula/simplify/wnext.hpp"
#include "formula/simplify/until.hpp"
#include "formula/simplify/release.hpp"
#include "formula/builder.hpp"
#include "formula/formula.hpp"

namespace Cosy {

Formula* FormulaSimplifier::simplify(Formula* formula, FormulaBuilder& builder) {
    if (!formula) return nullptr;

    Operator op = formula->op();

    // Constants and literals don't need simplification
    if (op == Operator::True || op == Operator::False || op == Operator::Literal) {
        return formula;
    }

    // Handle unary operators
    if (!formula->is_binary()) {
        // Delegate to operator-specific simplifiers
        if (op == Operator::Next) {
            return NextSimplifier::simplify(formula, builder);
        }
        if (op == Operator::WNext) {
            return WNextSimplifier::simplify(formula, builder);
        }

        // For other unary operators (Not), recursively simplify child
        Formula* right = formula->right() ? simplify(formula->right(), builder) : nullptr;
        return builder.make_unary(op, right);
    }

    // Delegate to operator-specific simplifiers
    // Note: AndSimplifier and OrSimplifier handle recursive simplification internally
    if (op == Operator::And) {
        return AndSimplifier::simplify(formula, builder);
    } else if (op == Operator::Or) {
        return OrSimplifier::simplify(formula, builder);
    } else if (op == Operator::Until) {
        return UntilSimplifier::simplify(formula, builder);
    } else if (op == Operator::Release) {
        return ReleaseSimplifier::simplify(formula, builder);
    }

    // For remaining binary operators, recursively simplify children first
    Formula* left = formula->left() ? simplify(formula->left(), builder) : nullptr;
    Formula* right = formula->right() ? simplify(formula->right(), builder) : nullptr;

    return builder.make_binary(op, left, right);
}

} // namespace Cosy
