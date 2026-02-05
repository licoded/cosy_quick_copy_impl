#include "formula/simplify.hpp"
#include "formula/builder.hpp"
#include "formula/formula.hpp"
#include <set>

namespace Cosy {

namespace {

/**
 * @brief Collect binary operator terms into a set (flatten chains)
 *
 * Flattens nested AND/OR structures recursively: `(a & (b & c))` → `{a, b, c}`
 * Uses std::set for deterministic ordering (crucial for hash consing)
 */
void collect_terms(Formula* f, std::set<Formula*>& terms, Operator op) {
    if (!f) return;

    if (f->op() == op) {
        // Flatten chain: collect both sides
        collect_terms(f->left(), terms, op);
        collect_terms(f->right(), terms, op);
    } else {
        // Leaf term
        terms.insert(f);
    }
}

/**
 * @brief Rebuild a binary operator chain from a set of terms
 *
 * Builds right-leaning chain: {a, b, c} → op(a, op(b, c))
 * Deterministic ordering from std::set ensures consistent hash consing
 */
Formula* rebuild_chain(FormulaBuilder& builder, const std::set<Formula*>& terms, Operator op) {
    if (terms.empty()) {
        // Identity element
        return (op == Operator::And) ? builder.make_true() : builder.make_false();
    }

    if (terms.size() == 1) {
        return *terms.begin();
    }

    // Build right-leaning chain
    auto it = terms.begin();
    Formula* result = *it;
    ++it;

    for (; it != terms.end(); ++it) {
        result = builder.make_binary(op, result, *it);
    }

    return result;
}

} // anonymous namespace

Formula* simplify(Formula* formula, FormulaBuilder& builder) {
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
        return builder.make_unary(op, right);
    }

    // Handle binary operators with canonicalization
    if (op == Operator::And || op == Operator::Or) {
        std::set<Formula*> terms;

        // Phase 1: Flatten nested structures
        collect_terms(left, terms, op);
        collect_terms(right, terms, op);

        // Phase 2: Apply simplification rules
        if (op == Operator::And) {
            // Check for False (dominance: False & anything → False)
            Formula* false_f = builder.make_false();
            if (terms.find(false_f) != terms.end()) {
                return false_f;
            }

            // Remove True (identity: True & x → x)
            Formula* true_f = builder.make_true();
            terms.erase(true_f);

            // TODO: Check for complementary literals (a & !a → False)
        } else { // Or
            // Check for True (dominance: True | anything → True)
            Formula* true_f = builder.make_true();
            if (terms.find(true_f) != terms.end()) {
                return true_f;
            }

            // Remove False (identity: False | x → x)
            Formula* false_f = builder.make_false();
            terms.erase(false_f);

            // TODO: Check for complementary literals (a | !a → True)
        }

        // Phase 3: Rebuild canonicalized chain
        return rebuild_chain(builder, terms, op);
    }

    // For other binary operators (Until, Release), just rebuild with simplified children
    // TODO: Add simplification rules for Until/Release
    return builder.make_binary(op, left, right);
}

} // namespace Cosy
