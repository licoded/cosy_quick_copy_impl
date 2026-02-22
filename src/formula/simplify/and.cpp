#include "formula/simplify/and.hpp"
#include "formula/simplify/simplifier.hpp"
#include "formula/simplify/util.hpp"
#include "formula/builder.hpp"
#include "formula/formula.hpp"
#include <set>

namespace Cosy {

Formula* AndSimplifier::simplify(Formula* formula, FormulaBuilder& builder, bool dep) {
    std::set<Formula*> terms;

    // Phase 1: Initial collection (flatten nested AND from original tree)
    SimplifyUtil::collect_binary_terms(formula, terms, Operator::And);

    // Check for False (dominance: False & anything → False)
    Formula* false_f = builder.make_false();
    if (terms.find(false_f) != terms.end()) {
        return false_f;
    }

    // Phase 2: Simplify each term and expand any new AND formulas
    std::set<Formula*> new_terms;
    if (dep) {
        for (Formula* f : terms) {
            Formula* simplified = FormulaSimplifier::simplify(f, builder);

            // If simplification produced an AND, expand it
            if (simplified->op() == Operator::And) {
                SimplifyUtil::collect_binary_terms(simplified, new_terms, Operator::And);
            } else if (simplified->op() != Operator::True) {
                // Skip True (identity for AND)
                new_terms.insert(simplified);
            }
        }

        // Check for False again after simplification
        if (new_terms.find(false_f) != new_terms.end()) {
            return false_f;
        }

        swap(terms, new_terms); // Reuse set for next phase
    }

    // Phase 3: Check for conflicts (a & !a)
    if (SimplifyUtil::has_complementary_literals(terms, builder)) {
        return false_f;
    }

    // Rebuild chain from deduplicated terms
    return builder.formula_reduce(Operator::And, std::vector<Formula*>(terms.begin(), terms.end()), false);
}

} // namespace Cosy
