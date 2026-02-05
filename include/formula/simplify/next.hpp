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
     * @param formula The Next formula to simplify
     * @param builder FormulaBuilder for creating formulas
     * @return Simplified formula
     */
    static Formula* simplify(Formula* formula, FormulaBuilder& builder);
};

} // namespace Cosy
