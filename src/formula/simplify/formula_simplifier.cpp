#include "formula/simplify/formula_simplifier.hpp"
#include "formula/simplify/and_simplifier.hpp"
#include "formula/simplify/or_simplifier.hpp"
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

    // Recursively simplify children first
    Formula* left = formula->left() ? simplify(formula->left(), builder) : nullptr;
    Formula* right = formula->right() ? simplify(formula->right(), builder) : nullptr;

    // Handle unary operators
    if (!formula->is_binary()) {
        // For unary operators, just rebuild with simplified child
        // TODO: Add operator-specific simplification for Next, etc.
        return builder.make_unary(op, right);
    }

    // Delegate to operator-specific simplifiers
    if (op == Operator::And) {
        // Rebuild with simplified children first
        Formula* simplified_and = builder.make_binary(Operator::And, left, right);
        return AndSimplifier::simplify(simplified_and, builder);
    } else if (op == Operator::Or) {
        // Rebuild with simplified children first
        Formula* simplified_or = builder.make_binary(Operator::Or, left, right);
        return OrSimplifier::simplify(simplified_or, builder);
    }

    // For other binary operators (Until, Release), just rebuild with simplified children
    // TODO: Add simplification rules for Until/Release
    return builder.make_binary(op, left, right);
}

} // namespace Cosy
