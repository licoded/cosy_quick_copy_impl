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
    Formula* true_f = builder.make_true();
    terms.erase(true_f); // Remove True (identity for AND)
    // Check complementary literals (a & !a)
    if (SimplifyUtil::has_complementary_literals(terms, builder)) {
        return false_f;
    }

    // Phase 2: Simplify each term and expand any new AND formulas
    std::set<Formula*> new_terms;
    if (dep) {
        for (Formula* f : terms) {
            Formula* simplified = FormulaSimplifier::simplify(f, builder);

            switch (simplified->op()) {
                case Operator::And:
                    // If simplification produced an AND, expand it
                    // TODO: 这里直接放入 new_terms 会不会少化简了?
                    SimplifyUtil::collect_binary_terms(simplified, new_terms, Operator::And);
                    break;
                case Operator::True:
                    // True is identity for AND, skip it
                    break;
                case Operator::False:
                    // False dominates everything in AND
                    return false_f;
                default:
                    new_terms.insert(simplified);
                    break;
            }
        }

        // Check complementary literals (a & !a)
        if (SimplifyUtil::has_complementary_literals(terms, builder)) {
            return false_f;
        }

        swap(terms, new_terms); // Reuse set for next phase
    }

    // Rebuild chain from deduplicated terms
    return builder.formula_reduce(Operator::And, std::vector<Formula*>(terms.begin(), terms.end()), false);
}

} // namespace Cosy
