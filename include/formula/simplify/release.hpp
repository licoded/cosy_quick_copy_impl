#pragma once

#include "formula/operator.hpp"

namespace Cosy {

class Formula;
class FormulaBuilder;

/**
 * @brief Simplifier for Release (R) operator
 *
 * Simplification rules:
 * - True R a → a
 * - a R False → False
 * - a R True → True
 * - a R (a & b) → a & b (right absorption)
 * - (a | b) R a → a (left absorption)
 * - !a R a → False R a (not absorption)
 */
class ReleaseSimplifier {
public:
    /**
     * @brief Simplify a Release formula
     *
     * @param formula The Release formula to simplify
     * @param builder FormulaBuilder for creating formulas
     * @return Simplified formula
     */
    static Formula* simplify(Formula* formula, FormulaBuilder& builder);
};

} // namespace Cosy