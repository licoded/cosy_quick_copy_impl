#include "formula/simplify/and_simplifier.hpp"
#include "formula/simplify/formula_simplifier.hpp"
#include "formula/simplify/util.hpp"
#include "formula/builder.hpp"
#include "formula/formula.hpp"
#include <set>

namespace Cosy {

Formula* AndSimplifier::simplify(Formula* left, Formula* right, FormulaBuilder& builder) {
    std::set<Formula*> terms;

    // Phase 1: Initial collection (flatten nested AND from original tree)
    SimplifyUtil::collect_binary_terms(left, terms, Operator::And);
    SimplifyUtil::collect_binary_terms(right, terms, Operator::And);

    // Check for False (dominance: False & anything → False)
    Formula* false_f = builder.make_false();
    if (terms.find(false_f) != terms.end()) {
        return false_f;
    }

    // Phase 2: Simplify each term and expand any new AND formulas
    std::set<Formula*> new_terms;
    for (Formula* f : terms) {
        Formula* simplified = FormulaSimplifier::simplify(f, builder);

        // If simplification produced an AND, expand it
        if (simplified->op() == Operator::And) {
            SimplifyUtil::collect_binary_terms(simplified->left(), new_terms, Operator::And);
            SimplifyUtil::collect_binary_terms(simplified->right(), new_terms, Operator::And);
        } else if (simplified->op() != Operator::True) {
            // Skip True (identity for AND)
            new_terms.insert(simplified);
        }
    }

    // Check for False again after simplification
    if (new_terms.find(false_f) != new_terms.end()) {
        return false_f;
    }

    // TODO: Phase 3: Check for conflicts (a & !a)

    // Rebuild chain from deduplicated terms
    return SimplifyUtil::rebuild_chain(builder, new_terms, Operator::And);
}

} // namespace Cosy
