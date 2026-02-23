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
    Formula* false_f = builder.make_false();
    terms.erase(false_f); // Remove False (identity for OR)
    // Check complementary literals (a | !a)
    if (SimplifyUtil::has_complementary_literals(terms, builder)) {
        return true_f;
    }

    // Phase 2: Simplify each term and expand any new OR formulas
    std::set<Formula*> new_terms;
    if (dep) {
        for (Formula* f : terms) {
            Formula* simplified = FormulaSimplifier::simplify(f, builder);

            switch (simplified->op()) {
                case Operator::Or:
                    // If simplification produced an Or, expand it
                    // TODO: 这里直接放入 new_terms 会不会少化简了?
                    SimplifyUtil::collect_binary_terms(simplified, new_terms, Operator::Or);
                    break;
                case Operator::True:
                    // True dominates everything in OR
                    return true_f;
                case Operator::False:
                    // False is identity for OR, skip it
                    break;
                default:
                    new_terms.insert(simplified);
                    break;
            }
        }

        // Check complementary literals (a | !a)
        if (SimplifyUtil::has_complementary_literals(new_terms, builder)) {
            return true_f;
        }

        swap(terms, new_terms); // Reuse set for next phase
    }

    // Rebuild chain from deduplicated terms
    return builder.formula_reduce(Operator::Or, std::vector<Formula*>(terms.begin(), terms.end()), false);
}

} // namespace Cosy
