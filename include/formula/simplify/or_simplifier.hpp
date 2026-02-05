#pragma once

#include <set>

namespace Cosy {

class Formula;
class FormulaBuilder;

/**
 * @brief Simplifier for OR operations
 *
 * Applies the following transformations:
 * - Flattening: (a | (b | c)) → {a, b, c}
 * - Commutativity: a | b = b | a (via deterministic ordering)
 * - Associativity: (a | b) | c = a | (b | c) (via flattening)
 * - Constant propagation: True | x → True
 * - Identity elimination: False | x → x
 *
 * TODO: Add complementary literal detection (a | !a → True)
 */
class OrSimplifier {
public:
    /**
     * @brief Simplify an OR formula
     *
     * @param formula The formula to simplify (must have op() == Operator::Or)
     * @param builder The formula builder for creating new formulas
     * @return Simplified formula
     */
    static Formula* simplify(Formula* formula, FormulaBuilder& builder);

private:
    /**
     * @brief Collect OR terms into a set (flatten chains)
     *
     * Flattens nested OR structures recursively: `(a | (b | c))` → `{a, b, c}`
     * Uses std::set for deterministic ordering (crucial for hash consing)
     */
    static void collect_terms(Formula* f, std::set<Formula*>& terms);

    /**
     * @brief Rebuild an OR chain from a set of terms
     *
     * Builds right-leaning chain: {a, b, c} → a | (b | c)
     * Deterministic ordering from std::set ensures consistent hash consing
     */
    static Formula* rebuild_chain(FormulaBuilder& builder, const std::set<Formula*>& terms);
};

} // namespace Cosy
