#include "formula/simplify/or.hpp"
#include "formula/simplify/simplifier.hpp"
#include "formula/simplify/util.hpp"
#include "formula/builder.hpp"
#include "formula/formula.hpp"
#include <set>

namespace Cosy {

Formula* OrSimplifier::simplify(Formula* left, Formula* right, FormulaBuilder& builder) {
    std::set<Formula*> terms;

    // Phase 1: Initial collection (flatten nested OR from original tree)
    SimplifyUtil::collect_binary_terms(left, terms, Operator::Or);
    SimplifyUtil::collect_binary_terms(right, terms, Operator::Or);

    // Check for True (dominance: True | anything → True)
    Formula* true_f = builder.make_true();
    if (terms.find(true_f) != terms.end()) {
        return true_f;
    }

    // Phase 2: Simplify each term and expand any new OR formulas
    std::set<Formula*> new_terms;
    for (Formula* f : terms) {
        Formula* simplified = FormulaSimplifier::simplify(f, builder);

        // If simplification produced an OR, expand it
        if (simplified->op() == Operator::Or) {
            SimplifyUtil::collect_binary_terms(simplified->left(), new_terms, Operator::Or);
            SimplifyUtil::collect_binary_terms(simplified->right(), new_terms, Operator::Or);
        } else if (simplified->op() != Operator::False) {
            // Skip False (identity for OR)
            new_terms.insert(simplified);
        }
    }

    // Check for True again after simplification
    if (new_terms.find(true_f) != new_terms.end()) {
        return true_f;
    }

    // Phase 3: Check for tautologies (a | !a)
    if (SimplifyUtil::has_complementary_literals(new_terms, builder)) {
        return true_f;
    }

    // Rebuild chain from deduplicated terms
    return SimplifyUtil::rebuild_chain(builder, new_terms, Operator::Or);
}

} // namespace Cosy
