#include "formula/simplify/util.hpp"
#include "formula/builder.hpp"
#include "formula/formula.hpp"

namespace Cosy {
namespace SimplifyUtil {

void collect_binary_terms(Formula* f, std::set<Formula*>& terms, Operator op) {
    if (!f) return;

    if (f->op() == op) {
        // Flatten chain: collect both sides
        collect_binary_terms(f->left(), terms, op);
        collect_binary_terms(f->right(), terms, op);
    } else {
        // Collect all terms (including True, False)
        terms.insert(f);
    }
}

Formula* rebuild_chain(FormulaBuilder& builder, const std::set<Formula*>& terms, Operator op) {
    if (terms.empty()) {
        // Identity element
        return (op == Operator::And) ? builder.make_true() : builder.make_false();
    }

    if (terms.size() == 1) {
        return *terms.begin();
    }

    // Build right-leaning chain: a & b & c → &(a, &(b, c))
    auto it = terms.begin();
    Formula* result = *it;
    ++it;

    for (; it != terms.end(); ++it) {
        result = builder.make_binary(op, result, *it);
    }

    return result;
}

bool has_complementary_literals(const std::set<Formula*>& terms, FormulaBuilder& builder) {
    std::set<Formula*> positives; // v0, v1, ...
    std::set<Formula*> negatives; // !v0, !v1, ...

    for (Formula* f : terms) {
        if (f->op() == Operator::Not) {
            // Negative literal: !a
            if (positives.find(f->right()) != positives.end()) {
                return true; // a and !a both present
            }
            negatives.insert(f);
        } else if (f->op() == Operator::Literal) {
            // Positive literal: a
            // Check if !a exists by creating Not(a) and looking it up
            Formula* negated = builder.make_unary(Operator::Not, f);
            if (negatives.find(negated) != negatives.end()) {
                return true; // !a and a both present
            }
            positives.insert(f);
        }
        // Non-literal terms are ignored
        // (e.g., Next(a), (a & b) - these don't have simple complements)
    }
    return false;
}

} // namespace SimplifyUtil
} // namespace Cosy
