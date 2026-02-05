#pragma once

namespace Cosy {

class Formula;
class FormulaBuilder;

/**
 * @brief Simplifier for AND operations
 *
 * Process:
 * 1. Collect terms from both sides (flatten nested AND)
 * 2. Simplify each term recursively
 * 3. If simplification produces new AND, expand and re-collect
 * 4. Check for conflicts (a & !a) - TODO
 * 5. Rebuild flattened chain
 *
 * Applies the following transformations:
 * - Flattening: (a & (b & c)) → {a, b, c}
 * - Commutativity: a & b = b & a (via deterministic ordering)
 * - Associativity: (a & b) & c = a & (b & c) (via flattening)
 * - Constant propagation: False & x → False
 * - Identity elimination: True & x → x
 *
 * TODO: Add complementary literal detection (a & !a → False)
 */
class AndSimplifier {
public:
    /**
     * @brief Simplify an AND formula
     *
     * @param formula The AND formula to simplify
     * @param builder The formula builder for creating new formulas
     * @return Simplified formula
     */
    static Formula* simplify(Formula* formula, FormulaBuilder& builder);
};

} // namespace Cosy
