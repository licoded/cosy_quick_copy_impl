#include "formula/simplify/or.hpp"
#include "formula/simplify/simplifier.hpp"
#include "formula/simplify/util.hpp"
#include "formula/builder.hpp"
#include "formula/formula.hpp"
#include <set>

namespace Cosy {

Formula* OrSimplifier::simplify(Formula* formula, FormulaBuilder& builder, bool dep) {
    std::set<Formula*> terms;

    // Phase 1: Initial collection (flatten nested OR from original tree)
    SimplifyUtil::collect_binary_terms(formula, terms, Operator::Or);

    // Check for True (dominance: True | anything → True)
    Formula* true_f = builder.make_true();
    if (terms.find(true_f) != terms.end()) {
        return true_f;
    }

    // Phase 2: Simplify each term and expand any new OR formulas
    std::set<Formula*> new_terms;
    for (Formula* f : terms) {
        Formula* simplified = dep ? FormulaSimplifier::simplify(f, builder) : f;

        // If simplification produced an OR, expand it
        if (dep && simplified->op() == Operator::Or) {
            SimplifyUtil::collect_binary_terms(simplified, new_terms, Operator::Or);
        } else if (simplified->op() != Operator::False) {
            // Skip False (identity for OR)
            new_terms.insert(simplified);
        }
    }

    // Check for True again after simplification
    if (new_terms.find(true_f) != new_terms.end()) {
        return true_f;
    }

    swap(terms, new_terms); // Reuse set for next phase

    // Phase 3: Check for tautologies (a | !a)
    if (SimplifyUtil::has_complementary_literals(terms, builder)) {
        return true_f;
    }

    // Rebuild chain from deduplicated terms
    return builder.formula_reduce(Operator::Or, std::vector<Formula*>(terms.begin(), terms.end()), false);
}

} // namespace Cosy
