#pragma once

#include "formula/operator.hpp"

namespace Cosy {

class Formula;
class FormulaBuilder;

/**
 * @brief Simplifier for Next (X[!]) operator
 *
 * Simplification rules:
 * - X[!] False → False
 * - X[!] True → True (optional, currently disabled)
 */
class NextSimplifier {
public:
    /**
     * @brief Simplify a Next formula
     *
     * @param operand The operand of Next (not yet simplified)
     * @param builder FormulaBuilder for creating formulas
     * @return Simplified formula, or nullptr if no change
     */
    static Formula* simplify(Formula* operand, FormulaBuilder& builder);
};

} // namespace Cosy
