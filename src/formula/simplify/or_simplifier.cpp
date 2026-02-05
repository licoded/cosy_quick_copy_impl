#include "formula/simplify/or_simplifier.hpp"
#include "formula/builder.hpp"
#include "formula/formula.hpp"

namespace Cosy {

void OrSimplifier::collect_terms(Formula* f, std::set<Formula*>& terms) {
    if (!f) return;

    if (f->op() == Operator::Or) {
        // Flatten chain: collect both sides
        collect_terms(f->left(), terms);
        collect_terms(f->right(), terms);
    } else {
        // Leaf term
        terms.insert(f);
    }
}

Formula* OrSimplifier::rebuild_chain(FormulaBuilder& builder, const std::set<Formula*>& terms) {
    if (terms.empty()) {
        // Identity element for OR is False
        return builder.make_false();
    }

    if (terms.size() == 1) {
        return *terms.begin();
    }

    // Build right-leaning chain
    auto it = terms.begin();
    Formula* result = *it;
    ++it;

    for (; it != terms.end(); ++it) {
        result = builder.make_binary(Operator::Or, result, *it);
    }

    return result;
}

Formula* OrSimplifier::simplify(Formula* formula, FormulaBuilder& builder) {
    if (!formula || formula->op() != Operator::Or) {
        return formula;
    }

    std::set<Formula*> terms;

    // Phase 1: Flatten nested structures
    collect_terms(formula, terms);

    // Phase 2: Apply simplification rules
    // Check for True (dominance: True | anything → True)
    Formula* true_f = builder.make_true();
    if (terms.find(true_f) != terms.end()) {
        return true_f;
    }

    // Remove False (identity: False | x → x)
    Formula* false_f = builder.make_false();
    terms.erase(false_f);

    // TODO: Check for complementary literals (a | !a → True)

    // Phase 3: Rebuild canonicalized chain
    return rebuild_chain(builder, terms);
}

} // namespace Cosy
