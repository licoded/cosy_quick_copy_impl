#include "formula/simplify/and_simplifier.hpp"
#include "formula/builder.hpp"
#include "formula/formula.hpp"

namespace Cosy {

void AndSimplifier::collect_terms(Formula* f, std::set<Formula*>& terms) {
    if (!f) return;

    if (f->op() == Operator::And) {
        // Flatten chain: collect both sides
        collect_terms(f->left(), terms);
        collect_terms(f->right(), terms);
    } else {
        // Leaf term
        terms.insert(f);
    }
}

Formula* AndSimplifier::rebuild_chain(FormulaBuilder& builder, const std::set<Formula*>& terms) {
    if (terms.empty()) {
        // Identity element for AND is True
        return builder.make_true();
    }

    if (terms.size() == 1) {
        return *terms.begin();
    }

    // Build right-leaning chain
    auto it = terms.begin();
    Formula* result = *it;
    ++it;

    for (; it != terms.end(); ++it) {
        result = builder.make_binary(Operator::And, result, *it);
    }

    return result;
}

Formula* AndSimplifier::simplify(Formula* formula, FormulaBuilder& builder) {
    if (!formula || formula->op() != Operator::And) {
        return formula;
    }

    std::set<Formula*> terms;

    // Phase 1: Flatten nested structures
    collect_terms(formula, terms);

    // Phase 2: Apply simplification rules
    // Check for False (dominance: False & anything → False)
    Formula* false_f = builder.make_false();
    if (terms.find(false_f) != terms.end()) {
        return false_f;
    }

    // Remove True (identity: True & x → x)
    Formula* true_f = builder.make_true();
    terms.erase(true_f);

    // TODO: Check for complementary literals (a & !a → False)

    // Phase 3: Rebuild canonicalized chain
    return rebuild_chain(builder, terms);
}

} // namespace Cosy
