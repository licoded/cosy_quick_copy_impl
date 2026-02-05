#include "formula/simplify/simplifier.hpp"
#include "formula/simplify/and.hpp"
#include "formula/simplify/or.hpp"
#include "formula/simplify/next.hpp"
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
            return NextSimplifier::simplify(formula->right(), builder);
        }

        // For other unary operators (Not, WNext), recursively simplify child
        Formula* right = formula->right() ? simplify(formula->right(), builder) : nullptr;
        return builder.make_unary(op, right);
    }

    // Delegate to operator-specific simplifiers
    // Note: AndSimplifier and OrSimplifier handle recursive simplification internally
    if (op == Operator::And) {
        return AndSimplifier::simplify(formula->left(), formula->right(), builder);
    } else if (op == Operator::Or) {
        return OrSimplifier::simplify(formula->left(), formula->right(), builder);
    }

    // For other binary operators (Until, Release), recursively simplify children first
    Formula* left = formula->left() ? simplify(formula->left(), builder) : nullptr;
    Formula* right = formula->right() ? simplify(formula->right(), builder) : nullptr;

    // TODO: Add simplification rules for Until/Release
    return builder.make_binary(op, left, right);
}

} // namespace Cosy
